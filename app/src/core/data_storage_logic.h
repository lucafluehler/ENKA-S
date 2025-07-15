#include <enkas/data/diagnostics.h>
#include <enkas/data/system.h>

#include <filesystem>
#include <string>

#include "settings.h"

class DataStorageLogic {
public:
    static void saveSettings(const std::filesystem::path& dir_path, const Settings& settings);
    static void saveSystemData(const std::filesystem::path& dir_path,
                               double time,
                               const enkas::data::System& system);
    static void saveDiagnosticsData(const std::filesystem::path& dir_path,
                                    double time,
                                    const enkas::data::Diagnostics& diagnostics);

private:
    static std::string getString(double value);
};