#pragma once

#include <enkas/data/diagnostics.h>
#include <enkas/data/system.h>

#include <filesystem>
#include <optional>
#include <vector>

#include "core/settings.h"

struct SystemFrame {
    double timestamp;
    enkas::data::System system;
};

struct DiagnosticsSeries {
    std::vector<double> timestamps;
    std::vector<enkas::data::Diagnostics> diagnostics_data;
};

class FileParseLogic {
public:
    /**
     * @brief Reads the settings from a CSV file.
     * @param file_path The path to the CSV file containing settings data.
     * @return An optional Settings object containing the settings data.
     */
    static std::optional<Settings> parseSettings(const std::filesystem::path& file_path);

    /**
     * @brief Reads the system frame just after the specified timestamp from the CSV file.
     * @param previous_timestamp The timestamp after which to read the next frame. If 0.0, reads the
     * first frame.
     * @return An optional Frame object containing the timestamp and system data.
     */
    static std::optional<SystemFrame> parseNextSystemFrame(const std::filesystem::path& file_path,
                                                           double previous_timestamp = 0.0);

    /**
     * @brief Reads the timestamps of a system file.
     * @return An optional System object containing the initial system data.
     */
    static std::optional<std::vector<double>> parseSystemTimestamps(
        const std::filesystem::path& file_path);

    /**
     * @brief Reads the diagnostics data series from the CSV file.
     * @return An optional DiagnosticsSeries object containing timestamps and diagnostics data.
     */
    static std::optional<DiagnosticsSeries> parseDiagnosticsSeries(
        const std::filesystem::path& file_path);
};
