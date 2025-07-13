#include "file_check_logic.h"

#include <filesystem>
#include <fstream>
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
    constexpr std::string_view EXPECTED_HEADER = "time,pos_x,pos_y,pos_z,vel_x,vel_y,vel_z,mass";

    std::ifstream file(file_path);
    if (!file.is_open()) {
        return false;
    }

    std::string line;

    // Read header
    if (!std::getline(file, line)) {
        return false;
    }
    if (line != EXPECTED_HEADER) {
        return false;
    }

    const size_t expected_field_count = std::ranges::count(EXPECTED_HEADER, ',') + 1;

    // Check each line in the body
    while (std::getline(file, line)) {
        std::istringstream line_stream(line);
        std::string field;
        size_t field_count = 0;

        while (std::getline(line_stream, field, ',')) {
            ++field_count;
            try {
                [[maybe_unused]] double value = std::stod(field);
            } catch (...) {
                return false;
            }
        }

        if (field_count != expected_field_count) {
            return false;
        }
    }

    return true;
}

bool FileCheckLogic::checkDiagnosticsFile(const std::filesystem::path& file_path) {
    constexpr std::string_view EXPECTED_HEADER =
        "time,e_kin,e_pot,L_tot,"
        "com_pos_x,com_pos_y,com_pos_z,"
        "com_vel_x,com_vel_y,com_vel_z,"
        "r_vir,ms_vel,t_cr";

    std::ifstream file(file_path);
    if (!file.is_open()) {
        return false;
    }

    std::string line;

    // Read header
    if (!std::getline(file, line)) {
        return false;
    }
    if (line != EXPECTED_HEADER) {
        return false;
    }

    const size_t expected_field_count = std::ranges::count(EXPECTED_HEADER, ',') + 1;

    // Read data lines
    while (std::getline(file, line)) {
        std::istringstream line_stream(line);
        std::string field;
        size_t field_count = 0;

        while (std::getline(line_stream, field, ',')) {
            ++field_count;
            try {
                [[maybe_unused]] double value = std::stod(field);
            } catch (...) {
                return false;
            }
        }

        if (field_count != expected_field_count) {
            return false;
        }
    }

    return true;
}
