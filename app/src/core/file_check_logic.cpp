#include "file_check_logic.h"

#include <csv-parser/csv.hpp>
#include <filesystem>
#include <fstream>
#include <json/json.hpp>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "settings.h"

bool FileCheckLogic::checkSettingsFile(const std::filesystem::path& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return false;
    }

    std::string header_line, value_line;
    if (!std::getline(file, header_line) || !std::getline(file, value_line)) {
        return false;
    }

    // Tokenize header and values
    std::istringstream header_stream(header_line);
    std::istringstream value_stream(value_line);
    std::vector<std::string> identifiers;
    std::vector<std::string> values;
    std::string token;

    std::unordered_set<std::string> seen;

    // Remove duplicates in identifiers
    while (std::getline(header_stream, token, ',')) {
        if (seen.insert(token).second) {
            identifiers.push_back(token);
        }
    }

    while (std::getline(value_stream, token, ',')) {
        values.push_back(token);
    }

    if (identifiers.empty() || values.empty() || identifiers.size() != values.size()) {
        return false;
    }

    Settings settings_template;
    for (size_t i = 0; i < identifiers.size(); ++i) {
        const std::string& key = identifiers[i];
        const std::string& value = values[i];
        if (!settings_template.setValue(key, value)) {
            return false;
        }
    }

    return true;
}

bool FileCheckLogic::checkSystemFile(const std::filesystem::path& file_path) {
    static const std::vector<std::string> expected_columns = {
        "time", "pos_x", "pos_y", "pos_z", "vel_x", "vel_y", "vel_z", "mass"};

    if (!std::filesystem::exists(file_path)) {
        return false;
    }

    try {
        auto format = csv::CSVFormat().header_row(0);
        csv::CSVReader reader(file_path.string(), format);

        if (reader.get_col_names() != expected_columns) {
            return false;
        }

        for (const auto& row : reader) {
            for (const auto& col_name : expected_columns) {
                [[maybe_unused]] double val = row[col_name].get<double>();
            }
        }

        return true;

    } catch (const std::exception& e) {
        return false;
    }
}

bool FileCheckLogic::checkDiagnosticsFile(const std::filesystem::path& file_path) {
    static const std::vector<std::string> expected_columns = {"time",
                                                              "e_kin",
                                                              "e_pot",
                                                              "L_tot",
                                                              "com_pos_x",
                                                              "com_pos_y",
                                                              "com_pos_z",
                                                              "com_vel_x",
                                                              "com_vel_y",
                                                              "com_vel_z",
                                                              "r_vir",
                                                              "ms_vel",
                                                              "t_cr"};

    if (!std::filesystem::exists(file_path)) {
        return false;
    }

    try {
        auto format =
            csv::CSVFormat().header_row(0).variable_columns(csv::VariableColumnPolicy::THROW);

        csv::CSVReader reader(file_path.string(), format);

        if (reader.get_col_names() != expected_columns) {
            return false;
        }

        for (const auto& row : reader) {
            for (const auto& col_name : expected_columns) {
                [[maybe_unused]] double value = row[col_name].get<double>();
            }
        }

        return true;

    } catch (const std::exception& e) {
        return false;
    }
}
