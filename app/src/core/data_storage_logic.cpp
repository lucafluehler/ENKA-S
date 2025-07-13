#include "data_storage_logic.h"

#include <enkas/generation/generation_method.h>
#include <enkas/simulation/simulation_method.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

std::string DataStorageLogic::getString(double value, int precision) {
    std::ostringstream stream;
    stream.precision(precision);
    stream << std::fixed << value;
    return stream.str();
}

void DataStorageLogic::saveSettings(const std::filesystem::path& dir_path,
                                    const Settings& settings) {
    // Create directory if it does not exist
    std::filesystem::create_directories(dir_path);

    // Open file for overwriting
    std::filesystem::path file_path = dir_path / "settings.csv";
    std::ofstream file(file_path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) return;

    // Header
    const auto identifiers = settings.getIdentifiers();
    for (size_t i = 0; i < identifiers.size(); ++i) {
        file << identifiers[i];
        if (i + 1 < identifiers.size()) file << ',';
    }
    file << '\n';

    // Body
    for (size_t i = 0; i < identifiers.size(); ++i) {
        const auto& id = identifiers[i];
        const Setting& setting = settings.getSetting(id);
        std::string v = Setting::variantToString(setting.value);

        if (setting.type == Setting::Type::File) {
            std::filesystem::path p(v);
            v = p.parent_path().filename().string();
        }

        file << v;
        if (i + 1 < identifiers.size()) file << ',';
    }
    file << '\n';
}

void DataStorageLogic::saveSystemData(const std::filesystem::path& dir_path,
                                      double time,
                                      const enkas::data::System& system) {
    // Create directory if it does not exist
    std::filesystem::create_directories(dir_path);

    // If file is new, add a header first
    const auto file_path = dir_path / "system.csv";
    const bool new_file = !std::filesystem::exists(file_path);

    // Open the file for appending
    std::ofstream file(dir_path / "system.csv", std::ios::app);
    if (!file.is_open()) return;

    // Header
    if (new_file) {
        file << "time,pos_x,pos_y,pos_z,vel_x,vel_y,vel_z,mass" << std::endl;
    }

    // Body
    for (size_t i = 0; i < system.positions.size(); ++i) {
        file << getString(time) << "," << getString(system.positions[i].x) << ","
             << getString(system.positions[i].y) << "," << getString(system.positions[i].z) << ","
             << getString(system.velocities[i].x) << "," << getString(system.velocities[i].y) << ","
             << getString(system.velocities[i].z) << "," << getString(system.masses[i])
             << std::endl;
    }
}
