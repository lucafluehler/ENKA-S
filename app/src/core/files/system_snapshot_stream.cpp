#include "system_snapshot_stream.h"

#include <enkas/logging/logger.h>

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>

#include "core/files/file_constants.h"

SystemSnapshotStream::SystemSnapshotStream(std::filesystem::path file_path)
    : file_path_(std::move(file_path)) {
    // Set iterator to a known invalid state initially
    current_index_iterator_ = index_.end();
}

SystemSnapshotStream::~SystemSnapshotStream() {
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
}

bool SystemSnapshotStream::initialize() {
    if (is_initialized_) {
        return true;
    }

    if (!buildIndex()) {
        ENKAS_LOG_ERROR("Failed to build index for file: {}", file_path_.string());
        return false;
    }

    // After a successful index, open the main stream for reading snapshots.
    file_stream_.open(file_path_, std::ios::binary);
    if (!file_stream_.is_open()) {
        ENKAS_LOG_ERROR("Failed to open system file for reading: {}", file_path_.string());
        index_.clear();
        return false;
    }

    // Set the current position to "before the beginning" to be used by getNextSnapshot.
    current_index_iterator_ = index_.end();

    is_initialized_ = true;
    ENKAS_LOG_INFO("Successfully indexed {} snapshots from {}", index_.size(), file_path_.string());
    return true;
}

bool SystemSnapshotStream::buildIndex() {
    std::ifstream index_stream(file_path_, std::ios::binary);
    if (!index_stream.is_open()) {
        ENKAS_LOG_ERROR("Unable to build index. Could not open file: {}", file_path_.string());
        return false;
    }

    // --- Header Parsing ---
    std::string header_line;
    if (!std::getline(index_stream, header_line)) {
        ENKAS_LOG_ERROR("File is empty or failed to read header: {}", file_path_.string());
        return false;
    }
    // Handle potential Windows line endings (\r\n)
    if (!header_line.empty() && header_line.back() == '\r') {
        header_line.pop_back();
    }

    std::vector<std::string> headers;
    std::stringstream header_ss(header_line);
    std::string field;
    while (std::getline(header_ss, field, ',')) {
        headers.push_back(field);
    }

    if (headers.empty()) {
        ENKAS_LOG_ERROR("Header line appears to be empty or malformed: {}", file_path_.string());
        return false;
    }

    if (headers != csv_headers::system) {
        ENKAS_LOG_ERROR("Invalid CSV header in file: {}", file_path_.string());
        return false;
    }

    // Build the map from column name to column index for fast lookups
    column_indices_.clear();
    for (size_t i = 0; i < headers.size(); ++i) {
        column_indices_[headers[i]] = i;
    }

    // Verify that the 'time' column exists, as it's essential for indexing
    if (column_indices_.find("time") == column_indices_.end()) {
        ENKAS_LOG_ERROR("Header is missing required 'time' column in file: {}",
                        file_path_.string());
        return false;
    }
    const size_t time_idx = column_indices_.at("time");

    // --- Data Indexing ---
    std::streampos first_data_pos = index_stream.tellg();
    if (first_data_pos == -1) {
        ENKAS_LOG_WARNING("File contains only a header, no data to index: {}", file_path_.string());
        return true;  // Not an error, just an empty file
    }

    std::string line;
    double last_timestamp = -1.0;
    std::streampos current_snapshot_start_pos = first_data_pos;
    int row_count = 0;

    std::streampos current_line_start_pos = first_data_pos;
    while (std::getline(index_stream, line)) {
        std::streampos next_line_start_pos = index_stream.tellg();

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) {  // Check for empty lines after stripping \r
            current_line_start_pos = next_line_start_pos;
            continue;
        }

        try {
            std::stringstream line_ss(line);
            std::vector<std::string> values;
            std::string value;
            while (std::getline(line_ss, value, ',')) {
                values.push_back(value);
            }

            if (values.size() <= time_idx) {
                ENKAS_LOG_WARNING("Skipping malformed row during indexing (not enough columns): {}",
                                  line);
                current_line_start_pos = next_line_start_pos;
                continue;
            }

            double current_timestamp = std::stod(values[time_idx]);

            if (last_timestamp != -1.0 && current_timestamp != last_timestamp) {
                index_[last_timestamp] = {current_snapshot_start_pos, row_count};
                current_snapshot_start_pos = current_line_start_pos;
                row_count = 0;
            }
            last_timestamp = current_timestamp;
            row_count++;

        } catch (const std::invalid_argument& e) {
            ENKAS_LOG_WARNING(
                "Skipping malformed line during indexing (invalid number): {}. Error: {}",
                line,
                e.what());
        } catch (const std::out_of_range& e) {
            ENKAS_LOG_WARNING(
                "Skipping malformed line during indexing (number out of range): {}. Error: {}",
                line,
                e.what());
        }
        current_line_start_pos = next_line_start_pos;
    }

    // Store the very last snapshot in the file
    if (row_count > 0) {
        index_[last_timestamp] = {current_snapshot_start_pos, row_count};
    }

    return !index_.empty();
}

std::optional<SystemSnapshot> SystemSnapshotStream::getSnapshotAtFraction(double mu) {
    if (!is_initialized_) return std::nullopt;

    if (mu < 0.0 || mu > 1.0) {
        ENKAS_LOG_ERROR("Fraction must be between 0.0 and 1.0, got: {}", mu);
        return std::nullopt;
    }

    const std::size_t idx = static_cast<std::size_t>(std::floor(mu * (index_.size() - 1)));

    // Move iterator to the desired position
    auto it = std::next(index_.cbegin(), idx);
    current_index_iterator_ = it;

    return parseSnapshotFromIndex(it->second, it->first);
}

std::optional<SystemSnapshot> SystemSnapshotStream::getFirstSnapshot() {
    if (!is_initialized_ || index_.empty()) return std::nullopt;
    current_index_iterator_ = index_.cbegin();
    return parseSnapshotFromIndex(current_index_iterator_->second, current_index_iterator_->first);
}

std::optional<SystemSnapshot> SystemSnapshotStream::getNextSnapshot() {
    if (!is_initialized_ || index_.empty()) return std::nullopt;

    if (current_index_iterator_ == index_.end()) {
        current_index_iterator_ = index_.begin();
    } else {
        ++current_index_iterator_;
    }

    if (current_index_iterator_ == index_.end()) {
        return std::nullopt;
    }

    return parseSnapshotFromIndex(current_index_iterator_->second, current_index_iterator_->first);
}

std::optional<SystemSnapshot> SystemSnapshotStream::getPrecedingSnapshot(double timestamp) {
    if (!is_initialized_ || index_.empty()) {
        return std::nullopt;
    }

    auto it = index_.lower_bound(timestamp);

    if (it == index_.begin()) {
        return std::nullopt;
    }

    --it;

    return parseSnapshotFromIndex(it->second, it->first);
}

std::vector<double> SystemSnapshotStream::getAllTimestamps() const {
    if (!is_initialized_) return {};
    std::vector<double> timestamps;
    timestamps.reserve(index_.size());
    for (const auto& [time, entry] : index_) {
        timestamps.push_back(time);
    }
    return timestamps;
}

std::optional<SystemSnapshot> SystemSnapshotStream::parseSnapshotFromIndex(
    const SnapshotIndexEntry& entry, double timestamp) {
    file_stream_.clear();
    file_stream_.seekg(entry.file_offset);

    if (!file_stream_) {
        ENKAS_LOG_ERROR("Failed to seek to offset {} in file {}",
                        static_cast<std::streamoff>(entry.file_offset),
                        file_path_.string());
        return std::nullopt;
    }

    // Get column indices once
    size_t pos_x_idx, pos_y_idx, pos_z_idx, vel_x_idx, vel_y_idx, vel_z_idx, mass_idx;
    try {
        pos_x_idx = column_indices_.at("pos_x");
        pos_y_idx = column_indices_.at("pos_y");
        pos_z_idx = column_indices_.at("pos_z");
        vel_x_idx = column_indices_.at("vel_x");
        vel_y_idx = column_indices_.at("vel_y");
        vel_z_idx = column_indices_.at("vel_z");
        mass_idx = column_indices_.at("mass");
    } catch (const std::out_of_range&) {
        ENKAS_LOG_ERROR(
            "CSV file header is missing one or more required columns (e.g. pos_x, vel_y, mass).");
        return std::nullopt;
    }
    const size_t max_idx =
        std::max({pos_x_idx, pos_y_idx, pos_z_idx, vel_x_idx, vel_y_idx, vel_z_idx, mass_idx});

    enkas::data::System system;
    system.positions.reserve(entry.row_count);
    system.velocities.reserve(entry.row_count);
    system.masses.reserve(entry.row_count);

    std::string line;
    for (int i = 0; i < entry.row_count; ++i) {
        if (!std::getline(file_stream_, line)) {
            ENKAS_LOG_ERROR(
                "Unexpected end of file for snapshot {}. Expected {} rows, but file ended after "
                "{}.",
                timestamp,
                entry.row_count,
                i);
            return std::nullopt;
        }

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        try {
            std::stringstream line_ss(line);
            std::vector<std::string> values;
            std::string value;
            while (std::getline(line_ss, value, ',')) {
                values.push_back(value);
            }

            if (values.size() <= max_idx) {
                ENKAS_LOG_ERROR(
                    "Malformed row in snapshot at timestamp {}: not enough columns. Line: {}",
                    timestamp,
                    line);
                return std::nullopt;
            }

            system.positions.emplace_back(std::stod(values[pos_x_idx]),
                                          std::stod(values[pos_y_idx]),
                                          std::stod(values[pos_z_idx]));
            system.velocities.emplace_back(std::stod(values[vel_x_idx]),
                                           std::stod(values[vel_y_idx]),
                                           std::stod(values[vel_z_idx]));
            system.masses.push_back(std::stod(values[mass_idx]));

        } catch (const std::invalid_argument& e) {
            ENKAS_LOG_ERROR("Error parsing value in snapshot at timestamp {}: {}. Line: {}",
                            timestamp,
                            e.what(),
                            line);
            return std::nullopt;
        } catch (const std::out_of_range& e) {
            ENKAS_LOG_ERROR("Value out of range in snapshot at timestamp {}: {}. Line: {}",
                            timestamp,
                            e.what(),
                            line);
            return std::nullopt;
        }
    }

    return SystemSnapshot{std::move(system), timestamp};
}

void SystemSnapshotStream::retreatIndexIterator() {
    if (!is_initialized_ || index_.empty() || current_index_iterator_ == index_.begin()) {
        current_index_iterator_ = index_.end();
        return;
    }

    if (current_index_iterator_ == index_.end()) {
        current_index_iterator_ = std::prev(index_.end());
    } else {
        --current_index_iterator_;
    }
}
