#pragma once

#include <enkas/data/system.h>

#include <csv-parser/csv.hpp>
#include <filesystem>
#include <optional>

struct Frame {
    double timestamp;
    enkas::data::System system;
};

class SystemParseLogic {
public:
    explicit SystemParseLogic();

    /**
     * @brief Initializes the parser with a CSV file.
     *
     * The parser will start reading from the beginning of the passed file after initialization.
     *
     * @param file_path The path to the CSV file containing system data.
     * @return True if initialization is successful, false otherwise.
     */
    bool init(const std::filesystem::path& file_path);

    /**
     * @brief Reads the next frame from the CSV file.
     * @return An optional Frame object containing the timestamp and system data.
     */
    std::optional<Frame> getNextFrame();

    /**
     * @brief Reads the first system data from the CSV file.
     * @return An optional System object containing the system data.
     */
    std::optional<enkas::data::System> getInitialSystem() const;

private:
    bool validateHeader();

    std::unique_ptr<csv::CSVReader> reader_;
    std::optional<csv::CSVRow> next_row_;

    std::optional<enkas::data::System> initial_system_cache_;
};
