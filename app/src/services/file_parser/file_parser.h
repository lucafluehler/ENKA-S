#pragma once

#include "services/file_parser/i_file_parser.h"

class FileParser : public IFileParser {
public:
    /**
     * @brief Reads the settings from a CSV file.
     * @param file_path The path to the CSV file containing settings data.
     * @return An optional Settings object containing the settings data.
     */
    std::optional<Settings> parseSettings(const std::filesystem::path& file_path) override;

    /**
     * @brief Reads the diagnostics data series from the CSV file.
     * @return An optional DiagnosticsSeries object containing timestamps and diagnostics data.
     */
    std::optional<DiagnosticsSeries> parseDiagnosticsSeries(
        const std::filesystem::path& file_path) override;

    /**
     * @brief Reads the initial system data from the CSV file.
     * @param file_path The path to the CSV file containing system data.
     * @return An optional System object containing the initial system data.
     */
    std::optional<enkas::data::System> parseInitialSystem(
        const std::filesystem::path& file_path) override;

    /**
     * @brief Counts the number of snapshots in the specified file.
     * @param file_path The path to the CSV file.
     * @return An optional integer containing the snapshot count.
     */
    std::optional<int> countSnapshots(const std::filesystem::path& file_path) override;

    /**
     * @brief Retrieves the simulation duration from the specified file.
     * @param file_path The path to the CSV file.
     * @return An optional double containing the simulation duration.
     */
    std::optional<double> retrieveSimulationDuration(
        const std::filesystem::path& file_path) override;
};
