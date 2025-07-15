#include "system_parse_logic.h"

SystemParseLogic::SystemParseLogic()
    : reader_(nullptr), next_row_(std::nullopt), initial_system_cache_(std::nullopt) {}

bool SystemParseLogic::init(const std::filesystem::path& file_path) {
    reader_ = nullptr;
    next_row_ = std::nullopt;
    initial_system_cache_ = std::nullopt;

    if (!std::filesystem::exists(file_path)) {
        return false;
    }

    try {
        // Read first frame to populate initial_system_cache_
        auto format =
            csv::CSVFormat().header_row(0).variable_columns(csv::VariableColumnPolicy::THROW);
        reader_ = std::make_unique<csv::CSVReader>(file_path.string(), format);

        if (!validateHeader()) {
            reader_ = nullptr;
            return false;
        }

        csv::CSVRow first_data_row;
        if (reader_->read_row(first_data_row)) {
            next_row_ = std::move(first_data_row);

            if (auto first_frame = getNextFrame()) {
                initial_system_cache_ = std::move(first_frame->system);
            }
        }

        // Reset reader to the beginning of the file for subsequent reads
        reader_ = std::make_unique<csv::CSVReader>(file_path.string(), format);
        csv::CSVRow temp_row;
        if (reader_->read_row(temp_row)) {
            next_row_ = std::move(temp_row);
        } else {
            next_row_ = std::nullopt;
        }

        return true;

    } catch (const std::exception& e) {
        reader_ = nullptr;
        return false;
    }
}

bool SystemParseLogic::validateHeader() {
    static const std::vector<std::string> expected_columns = {
        "time", "pos_x", "pos_y", "pos_z", "vel_x", "vel_y", "vel_z", "mass"};
    return reader_->get_col_names() == expected_columns;
}

std::optional<Frame> SystemParseLogic::getNextFrame() {
    if (!reader_ || !next_row_.has_value()) {
        return std::nullopt;
    }

    Frame current_frame;
    current_frame.timestamp = next_row_->operator[]("time").get<double>();

    while (next_row_.has_value()) {
        double row_time = next_row_->operator[]("time").get<double>();

        if (row_time != current_frame.timestamp) {
            // This row is for the next frame. Stop and leave it in next_row_.
            break;
        }

        const auto& row = *next_row_;
        current_frame.system.positions.emplace_back(
            row["pos_x"].get<double>(), row["pos_y"].get<double>(), row["pos_z"].get<double>());
        current_frame.system.velocities.emplace_back(
            row["vel_x"].get<double>(), row["vel_y"].get<double>(), row["vel_z"].get<double>());
        current_frame.system.masses.push_back(row["mass"].get<double>());

        csv::CSVRow temp_row;
        if (reader_->read_row(temp_row)) {
            next_row_ = std::move(temp_row);
        } else {
            next_row_.reset();  // End of file.
        }
    }

    return current_frame;
}

std::optional<enkas::data::System> SystemParseLogic::getInitialSystem() const {
    return initial_system_cache_;
}