#pragma once

#include <enkas/logging/logger.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief Manages the lifecycle of a single CSV file for file writing.
 *
 * It handles opening the file, writing headers and data, and closing the file.
 */
template <typename T>
class CsvFileWriter {
public:
    /**
     * @brief Constructs the writer, opens the file, and writes the header if new.
     * @param file_path The full path to the CSV file.
     * @param header The vector of strings for the CSV header.
     */
    CsvFileWriter(const std::filesystem::path& file_path, const std::vector<std::string>& header) {
        std::filesystem::create_directories(file_path.parent_path());

        const bool is_new_file = !std::filesystem::exists(file_path);

        file_stream_.open(file_path, std::ios::app);
        if (!file_stream_.is_open()) {
            ENKAS_LOG_CRITICAL("Failed to open file for writing: {}", file_path.string());
            throw std::runtime_error("Failed to open file for writing: " + file_path.string());
        }

        if (is_new_file && !header.empty()) {
            for (size_t i = 0; i < header.size(); ++i) {
                file_stream_ << header[i] << (i == header.size() - 1 ? "" : ",");
            }
            file_stream_ << '\n';
        }

        // Set precision for all subsequent writes
        file_stream_ << std::setprecision(std::numeric_limits<double>::max_digits10);
    }

    // Destructor automatically closes the file stream.
    ~CsvFileWriter() {
        if (file_stream_.is_open()) {
            file_stream_.close();
            ENKAS_LOG_INFO("Closed data file.");
        }
    }

    // Disable copy/move to prevent issues with the file handle.
    CsvFileWriter(const CsvFileWriter&) = delete;
    CsvFileWriter& operator=(const CsvFileWriter&) = delete;

    /**
     * @brief Writes a data object to the file.
     * Requires an `operator<<(std::ostream&, const T&)` to be defined for type T.
     */
    void write(const T& data) { file_stream_ << data; }

private:
    std::ofstream file_stream_;
};
