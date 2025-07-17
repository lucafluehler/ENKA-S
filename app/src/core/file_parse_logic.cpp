#include "file_parse_logic.h"

#include <enkas/data/system.h>

#include <csv-parser/csv.hpp>
#include <filesystem>
#include <json/json.hpp>
#include <vector>

#include "core/settings/settings.h"
#include "file_constants.h"

std::optional<Settings> FileParseLogic::parseSettings(const std::filesystem::path& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return std::nullopt;
    }

    try {
        nlohmann::json data;
        file >> data;

        return Settings::create(data);

    } catch (const nlohmann::json::parse_error&) {
        return std::nullopt;
    }
}

std::optional<SystemFrame> FileParseLogic::parseNextSystemFrame(
    const std::filesystem::path& file_path, double previous_timestamp) {
    if (!std::filesystem::exists(file_path)) return std::nullopt;

    try {
        auto format =
            csv::CSVFormat().header_row(0).variable_columns(csv::VariableColumnPolicy::THROW);
        csv::CSVReader reader(file_path.string(), format);

        // Header validation
        if (reader.get_col_names() != csv_headers::system) {
            return std::nullopt;
        }

        SystemFrame next_frame;
        bool found_target_frame = false;

        for (const auto& row : reader) {
            double current_time = row["time"].get<double>();

            if (current_time > previous_timestamp) {
                // This is the first row of the frame we want.
                if (!found_target_frame) {
                    found_target_frame = true;
                    next_frame.timestamp = current_time;
                }

                // If the time changes again, we've finished the frame.
                if (current_time != next_frame.timestamp) {
                    break;
                }

                // Add particle data to the frame
                next_frame.system.positions.emplace_back(row["pos_x"].get<double>(),
                                                         row["pos_y"].get<double>(),
                                                         row["pos_z"].get<double>());
                next_frame.system.velocities.emplace_back(row["vel_x"].get<double>(),
                                                          row["vel_y"].get<double>(),
                                                          row["vel_z"].get<double>());
                next_frame.system.masses.push_back(row["mass"].get<double>());
            }
        }

        if (found_target_frame) {
            return next_frame;
        }

        // No frame was found after the given timestamp.
        return std::nullopt;

    } catch (const std::exception& e) {
        return std::nullopt;
    }
}

std::optional<enkas::data::System> FileParseLogic::parseInitialSystem(
    const std::filesystem::path& file_path) {
    auto frame = parseNextSystemFrame(file_path, 0.0);
    if (frame) {
        return frame->system;
    }
    return std::nullopt;
}

std::optional<std::vector<double>> FileParseLogic::parseSystemTimestamps(
    const std::filesystem::path& file_path) {
    if (!std::filesystem::exists(file_path)) return std::nullopt;

    try {
        auto format = csv::CSVFormat().header_row(0);
        csv::CSVReader reader(file_path.string(), format);

        // Header validation
        if (reader.get_col_names() != csv_headers::system) {
            return std::nullopt;
        }

        std::set<double> unique_timestamps;
        for (const auto& row : reader) {
            unique_timestamps.insert(row["time"].get<double>());
        }

        return std::vector<double>(unique_timestamps.begin(), unique_timestamps.end());

    } catch (const std::exception& e) {
        return std::nullopt;
    }
}

std::optional<DiagnosticsSeries> FileParseLogic::parseDiagnosticsSeries(
    const std::filesystem::path& file_path) {
    if (!std::filesystem::exists(file_path)) return std::nullopt;

    try {
        auto format =
            csv::CSVFormat().header_row(0).variable_columns(csv::VariableColumnPolicy::THROW);
        csv::CSVReader reader(file_path.string(), format);

        // Header validation
        if (reader.get_col_names() != csv_headers::diagnostics) {
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
            return std::nullopt;  // File had header but no data
        }

        return series;

    } catch (const std::exception& e) {
        return std::nullopt;
    }
}
