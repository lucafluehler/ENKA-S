#include <enkas/data/diagnostics.h>
#include <enkas/data/system.h>

#include <filesystem>
#include <string>

#include "settings.h"

class DataStorageLogic {
public:
    /**
     * @brief Saves the settings to a JSON file in the specified directory.
     * @param dir_path The directory path where the settings file will be saved.
     * @param settings The settings to save.
     */
    static void saveSettings(const std::filesystem::path& dir_path, const Settings& settings);

    /**
     * @brief Saves the system data to a CSV file in the specified directory.
     * @param dir_path The directory path where the system data file will be saved.
     * @param time The timestamp of the system data.
     * @param system The system data to save.
     */
    static void saveSystemData(const std::filesystem::path& dir_path,
                               double time,
                               const enkas::data::System& system);

    /**
     * @brief Saves the diagnostics data to a CSV file in the specified directory.
     * @param dir_path The directory path where the diagnostics data file will be saved.
     * @param time The timestamp of the diagnostics data.
     * @param diagnostics The diagnostics data to save.
     */
    static void saveDiagnosticsData(const std::filesystem::path& dir_path,
                                    double time,
                                    const enkas::data::Diagnostics& diagnostics);

private:
    static std::string getString(double value);
};