#include "core/files/file_parse_logic.h"

#include <enkas/data/system.h>
#include <enkas/logging/logger.h>

#include <csv-parser/csv.hpp>
#include <filesystem>
#include <json/json.hpp>
#include <vector>

#include "core/dataflow/snapshot.h"
#include "core/files/file_constants.h"
#include "core/settings/settings.h"

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
            double time = row["time"].get<double>();

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

            series.push_back(Snapshot<enkas::data::Diagnostics>(std::move(d), time));
        }

        if (series.empty()) {
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

std::optional<enkas::data::System> FileParseLogic::parseInitialSystem(
    const std::filesystem::path& file_path) {
    if (!std::filesystem::exists(file_path)) {
        ENKAS_LOG_ERROR("File does not exist: {}", file_path.string());
        return std::nullopt;
    }

    try {
        auto format =
            csv::CSVFormat().header_row(0).variable_columns(csv::VariableColumnPolicy::THROW);
        csv::CSVReader reader(file_path.string(), format);

        if (reader.get_col_names() != csv_headers::system) {
            ENKAS_LOG_ERROR("Invalid CSV header in file: {}", file_path.string());
            return std::nullopt;
        }

        enkas::data::System system;
        std::optional<double> initial_timestamp;

        for (const auto& row : reader) {
            double current_time = row["time"].get<double>();

            if (!initial_timestamp.has_value()) {
                initial_timestamp = current_time;
            }

            if (current_time != *initial_timestamp) {
                break;
            }

            system.positions.emplace_back(
                row["pos_x"].get<double>(), row["pos_y"].get<double>(), row["pos_z"].get<double>());
            system.velocities.emplace_back(
                row["vel_x"].get<double>(), row["vel_y"].get<double>(), row["vel_z"].get<double>());
            system.masses.push_back(row["mass"].get<double>());
        }

        if (system.masses.empty()) {
            ENKAS_LOG_ERROR("No initial system data found in file: {}", file_path.string());
            return std::nullopt;
        }

        ENKAS_LOG_INFO("Successfully parsed initial system from file: {}", file_path.string());
        return system;

    } catch (const std::exception& e) {
        ENKAS_LOG_ERROR("Error occurred while parsing initial system: {}", e.what());
        return std::nullopt;
    }
}

std::optional<int> FileParseLogic::countSnapshots(const std::filesystem::path& file_path) {
    if (!std::filesystem::exists(file_path)) {
        ENKAS_LOG_ERROR("File does not exist: {}", file_path.string());
        return std::nullopt;
    }

    try {
        auto format =
            csv::CSVFormat().header_row(0).variable_columns(csv::VariableColumnPolicy::THROW);
        csv::CSVReader reader(file_path.string(), format);

        if (reader.get_col_names() != csv_headers::system) {
            ENKAS_LOG_ERROR("Invalid CSV header in file: {}", file_path.string());
            return std::nullopt;
        }

        std::set<double> unique_timestamps;
        for (const auto& row : reader) {
            unique_timestamps.insert(row["time"].get<double>());
        }

        ENKAS_LOG_INFO("Counted {} unique snapshots in file: {}",
                       unique_timestamps.size(),
                       file_path.string());
        return static_cast<int>(unique_timestamps.size());

    } catch (const std::exception& e) {
        ENKAS_LOG_ERROR("Error occurred while counting snapshots: {}", e.what());
        return std::nullopt;
    }
}

std::optional<double> FileParseLogic::retrieveSimulationDuration(
    const std::filesystem::path& file_path) {
    if (!std::filesystem::exists(file_path)) {
        ENKAS_LOG_ERROR("File does not exist: {}", file_path.string());
        return std::nullopt;
    }

    try {
        // We only care about the header row to find the 'time' column
        csv::CSVReader reader(file_path.string(), csv::CSVFormat().header_row(0));

        std::optional<double> last_timestamp;
        for (const auto& row : reader) {
            // This will continuously overwrite the value, leaving the timestamp from the last row
            last_timestamp = row["time"].get<double>();
        }

        if (!last_timestamp) {
            ENKAS_LOG_ERROR("No data rows with a 'time' column found in file: {}",
                            file_path.string());
            return std::nullopt;
        }

        ENKAS_LOG_INFO(
            "Retrieved simulation duration {} from file: {}", *last_timestamp, file_path.string());
        return last_timestamp;

    } catch (const std::exception& e) {
        ENKAS_LOG_ERROR(
            "Error occurred while retrieving simulation duration (is 'time' column missing?): {}",
            e.what());
        return std::nullopt;
    }
}
