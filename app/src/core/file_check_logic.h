#pragma once

#include <filesystem>

class FileCheckLogic {
public:
    static bool checkSettingsFile(const std::filesystem::path& file_path);
    static bool checkSystemFile(const std::filesystem::path& file_path);
    static bool checkDiagnosticsFile(const std::filesystem::path& file_path);
};
