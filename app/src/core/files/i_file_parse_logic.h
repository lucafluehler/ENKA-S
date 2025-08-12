#pragma once

#include <filesystem>
#include <optional>

#include "core/dataflow/snapshot.h"
#include "core/settings/settings.h"

class IFileParseLogic {
public:
    /**
     * @brief Reads the settings from a CSV file.
     * @param file_path The path to the CSV file containing settings data.
     * @return An optional Settings object containing the settings data.
     */
    virtual std::optional<Settings> parseSettings(const std::filesystem::path& file_path) = 0;

    /**
     * @brief Reads the diagnostics data series from the CSV file.
     * @return An optional DiagnosticsSeries object containing timestamps and diagnostics data.
     */
    virtual std::optional<DiagnosticsSeries> parseDiagnosticsSeries(
        const std::filesystem::path& file_path) = 0;

    /**
     * @brief Reads the initial system data from the CSV file.
     * @param file_path The path to the CSV file containing system data.
     * @return An optional System object containing the initial system data.
     */
    virtual std::optional<enkas::data::System> parseInitialSystem(
        const std::filesystem::path& file_path) = 0;

    /**
     * @brief Counts the number of snapshots in the specified file.
     * @param file_path The path to the CSV file.
     * @return An optional integer containing the snapshot count.
     */
    virtual std::optional<int> countSnapshots(const std::filesystem::path& file_path) = 0;

    /**
     * @brief Retrieves the simulation duration from the specified file.
     * @param file_path The path to the CSV file.
     * @return An optional double containing the simulation duration.
     */
    virtual std::optional<double> retrieveSimulationDuration(
        const std::filesystem::path& file_path) = 0;
};
