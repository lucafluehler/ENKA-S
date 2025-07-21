#include "file_parse_logic.h"

#include <enkas/data/system.h>
#include <enkas/logging/logger.h>

#include <csv-parser/csv.hpp>
#include <filesystem>
#include <json/json.hpp>
#include <vector>

#include "core/files/file_constants.h"
#include "core/settings/settings.h"
#include "core/snapshot.h"

std::optional<Settings> FileParseLogic::parseSettings(const std::filesystem::path& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        ENKAS_LOG_ERROR("Failed to open settings file: {}", file_path.string());
        return std::nullopt;
    }

    try {
        nlohmann::json data;
        file >> data;

        ENKAS_LOG_INFO("Successfully parsed settings file: {}", file_path.string());
        return Settings::create(data);

    } catch (const nlohmann::json::parse_error&) {
        ENKAS_LOG_ERROR("Failed to parse settings file: {}", file_path.string());
        return std::nullopt;
    }
}

std::optional<SystemSnapshot> FileParseLogic::parseNextSystemSnapshot(
    const std::filesystem::path& file_path, double previous_timestamp) {
    if (!std::filesystem::exists(file_path)) {
        ENKAS_LOG_ERROR("File does not exist: {}", file_path.string());
        return std::nullopt;
    }

    try {
        auto format =
            csv::CSVFormat().header_row(0).variable_columns(csv::VariableColumnPolicy::THROW);
        csv::CSVReader reader(file_path.string(), format);

        // Header validation
        if (reader.get_col_names() != csv_headers::system) {
            ENKAS_LOG_ERROR("Invalid CSV header in file: {}", file_path.string());
            return std::nullopt;
        }

        enkas::data::System system;
        double timestamp = 0.0;
        bool found_target_snapshot = false;

        for (const auto& row : reader) {
            double current_time = row["time"].get<double>();

            if (current_time > previous_timestamp) {
                // This is the first row of the snapshot we want.
                if (!found_target_snapshot) {
                    found_target_snapshot = true;
                    timestamp = current_time;
                }

                // If the time changes again, we've finished the snapshot.
                if (current_time != timestamp) {
                    break;
                }

                // Add particle data to the snapshot
                system.positions.emplace_back(row["pos_x"].get<double>(),
                                              row["pos_y"].get<double>(),
                                              row["pos_z"].get<double>());
                system.velocities.emplace_back(row["vel_x"].get<double>(),
                                               row["vel_y"].get<double>(),
                                               row["vel_z"].get<double>());
                system.masses.push_back(row["mass"].get<double>());
            }
        }

        if (found_target_snapshot) {
            // ENKAS_LOG_INFO("Successfully parsed system snapshot from file: {}",
            // file_path.string());
            return SystemSnapshot(std::move(system), timestamp);
        }

        // No snapshot was found after the given timestamp.
        ENKAS_LOG_INFO("No new system snapshot found after timestamp: {}", previous_timestamp);
        return std::nullopt;

    } catch (const std::exception& e) {
        ENKAS_LOG_ERROR("Error occurred while parsing system snapshot: {}", e.what());
        return std::nullopt;
    }
}

std::optional<enkas::data::System> FileParseLogic::parseInitialSystem(
    const std::filesystem::path& file_path) {
    auto snapshot = parseNextSystemSnapshot(file_path, 0.0);
    if (snapshot) {
        ENKAS_LOG_INFO("Successfully parsed initial system from file: {}", file_path.string());
        return snapshot->data;
    }
    ENKAS_LOG_ERROR("Failed to parse initial system from file: {}", file_path.string());
    return std::nullopt;
}

std::optional<std::vector<double>> FileParseLogic::parseSystemTimestamps(
    const std::filesystem::path& file_path) {
    if (!std::filesystem::exists(file_path)) {
        ENKAS_LOG_ERROR("File does not exist: {}", file_path.string());
        return std::nullopt;
    }

    try {
        auto format = csv::CSVFormat().header_row(0);
        csv::CSVReader reader(file_path.string(), format);

        // Header validation
        if (reader.get_col_names() != csv_headers::system) {
            ENKAS_LOG_ERROR("Invalid CSV header in file: {}", file_path.string());
            return std::nullopt;
        }

        std::set<double> unique_timestamps;
        for (const auto& row : reader) {
            unique_timestamps.insert(row["time"].get<double>());
        }

        if (unique_timestamps.empty()) {
            ENKAS_LOG_ERROR("No timestamps found in file: {}", file_path.string());
            return std::nullopt;  // No timestamps found
        }

        ENKAS_LOG_INFO("Successfully parsed {} unique timestamps from file: {}",
                       unique_timestamps.size(),
                       file_path.string());
        return std::vector<double>(unique_timestamps.begin(), unique_timestamps.end());

    } catch (const std::exception& e) {
        ENKAS_LOG_ERROR("Error occurred while parsing system timestamps: {}", e.what());
        return std::nullopt;
    }
}

std::optional<DiagnosticsSeries> FileParseLogic::parseDiagnosticsSeries(
    const std::filesystem::path& file_path) {
    if (!std::filesystem::exists(file_path)) {
        ENKAS_LOG_ERROR("File does not exist: {}", file_path.string());
        return std::nullopt;
    }

    try {
        auto format =
            csv::CSVFormat().header_row(0).variable_columns(csv::VariableColumnPolicy::THROW);
        csv::CSVReader reader(file_path.string(), format);

        // Header validation
        if (reader.get_col_names() != csv_headers::diagnostics) {
            ENKAS_LOG_ERROR("Invalid CSV header in file: {}", file_path.string());
            return std::nullopt;
        }

        DiagnosticsSeries series;
        for (const auto& row : reader) {
            series.timestamps.push_back(row["time"].get<double>());

            enkas::data::Diagnostics d;
            d.e_kin = row["e_kin"].get<double>();
            d.e_pot = row["e_pot"].get<double>();
            d.L_tot = row["L_tot"].get<double>();
            d.com_pos.x = row["com_pos_x"].get<double>();
            d.com_pos.y = row["com_pos_y"].get<double>();
            d.com_pos.z = row["com_pos_z"].get<double>();
            d.com_vel.x = row["com_vel_x"].get<double>();
            d.com_vel.y = row["com_vel_y"].get<double>();
            d.com_vel.z = row["com_vel_z"].get<double>();
            d.r_vir = row["r_vir"].get<double>();
            d.ms_vel = row["ms_vel"].get<double>();
            d.t_cr = row["t_cr"].get<double>();
            series.diagnostics_data.push_back(d);
        }

        if (series.timestamps.empty()) {
            ENKAS_LOG_ERROR("File had header but no data: {}", file_path.string());
            return std::nullopt;  // File had header but no data
        }

        ENKAS_LOG_INFO("Successfully parsed diagnostics series from file: {}", file_path.string());
        return series;

    } catch (const std::exception& e) {
        ENKAS_LOG_ERROR("Error occurred while parsing diagnostics series: {}", e.what());
        return std::nullopt;
    }
}
