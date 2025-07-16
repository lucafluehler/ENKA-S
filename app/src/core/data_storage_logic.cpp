#include "data_storage_logic.h"

#include <csv-parser/csv.hpp>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <json/json.hpp>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "core/file_constants.h"

std::string DataStorageLogic::getString(double value) {
    std::stringstream ss;
    ss << std::setprecision(std::numeric_limits<double>::max_digits10) << value;
    return ss.str();
}

void DataStorageLogic::saveSettings(const std::filesystem::path& dir_path,
                                    const Settings& settings) {
    // Create directory if it does not exist
    std::filesystem::create_directories(dir_path);

    // Convert Settings to JSON
    auto settings_json_opt = settings.toJson();
    if (!settings_json_opt) {
        return;  // Conversion failed
    }

    std::filesystem::path file_path = dir_path / file_names::settings;
    std::ofstream file(file_path);
    if (!file.is_open()) {
        return;
    }

    const int indent_level = 4;  // spaces
    file << settings_json_opt->dump(indent_level);
}

void DataStorageLogic::saveSystemData(const std::filesystem::path& dir_path,
                                      double time,
                                      const enkas::data::System& system) {
    // Create directory if it does not exist
    std::filesystem::create_directories(dir_path);

    // If file is new, add a header first
    const auto file_path = dir_path / file_names::system;
    const bool new_file = !std::filesystem::exists(file_path);

    // Open the file for appending
    std::ofstream file(file_path, std::ios::app);
    if (!file.is_open()) return;

    auto writer = csv::make_csv_writer(file);

    // Header
    if (new_file) {
        writer << csv_headers::system;
    }

    // Body
    for (size_t i = 0; i < system.count(); ++i) {
        writer << std::vector<std::string>{getString(time),
                                           getString(system.positions[i].x),
                                           getString(system.positions[i].y),
                                           getString(system.positions[i].z),
                                           getString(system.velocities[i].x),
                                           getString(system.velocities[i].y),
                                           getString(system.velocities[i].z),
                                           getString(system.masses[i])};
    }
}

void DataStorageLogic::saveDiagnosticsData(const std::filesystem::path& dir_path,
                                           double time,
                                           const enkas::data::Diagnostics& diagnostics) {
    // Create directory if it does not exist
    std::filesystem::create_directories(dir_path);

    // If file is new, add a header first
    const auto file_path = dir_path / file_names::diagnostics;
    const bool new_file = !std::filesystem::exists(file_path);

    // Open the file for appending
    std::ofstream file(file_path, std::ios::app);
    if (!file.is_open()) return;

    auto writer = csv::make_csv_writer(file);

    // Header
    if (new_file) {
        writer << csv_headers::diagnostics;
    }

    // Body
    writer << std::vector<std::string>{getString(time),
                                       getString(diagnostics.e_kin),
                                       getString(diagnostics.e_pot),
                                       getString(diagnostics.L_tot),
                                       getString(diagnostics.com_pos.x),
                                       getString(diagnostics.com_pos.y),
                                       getString(diagnostics.com_pos.z),
                                       getString(diagnostics.com_vel.x),
                                       getString(diagnostics.com_vel.y),
                                       getString(diagnostics.com_vel.z),
                                       getString(diagnostics.r_vir),
                                       getString(diagnostics.ms_vel),
                                       getString(diagnostics.t_cr)};
}
