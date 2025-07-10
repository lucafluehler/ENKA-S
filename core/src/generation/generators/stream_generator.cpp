#include <enkas/data/system.h>
#include <enkas/generation/generators/stream_generator.h>
#include <enkas/logging/logger.h>
#include <enkas/math/vector3d.h>

#include <istream>
#include <sstream>
#include <string>
#include <vector>

namespace enkas::generation {

StreamGenerator::StreamGenerator(std::istream& stream) : stream_(stream) {}

data::System StreamGenerator::createSystem() {
    auto& logger = logging::getLogger();
    logger.info(std::source_location::current(), "Creating system from stream...");

    data::System system;
    std::string line;

    if (!stream_) {
        logger.error(std::source_location::current(),
                     "Failed to read from stream. Stream is not valid.");
        return system;
    }

    // Skip header
    std::getline(stream_, line);

    // Load particles one by one :)
    int line_num = 1;
    while (std::getline(stream_, line)) {
        line_num++;

        if (line.empty()) {
            logger.debug(
                std::source_location::current(), "Skipping empty line at line {}", line_num);
            continue;
        }

        std::stringstream line_stream(line);
        std::string cell;
        std::vector<std::string> cells;

        // Split the line by commas into a vector of strings.
        while (std::getline(line_stream, cell, ',')) {
            cells.push_back(cell);
        }

        if (cells.size() < 7) {
            logger.warning(std::source_location::current(),
                           "Skipping line {} ({} columns, expected 7): '{}'",
                           line_num,
                           cells.size(),
                           line);
            continue;
        }

        try {
            math::Vector3D position = {
                std::stod(cells[0]), std::stod(cells[1]), std::stod(cells[2])};
            math::Vector3D velocity = {
                std::stod(cells[3]), std::stod(cells[4]), std::stod(cells[5])};

            system.positions.push_back(position);
            system.velocities.push_back(velocity);
            system.masses.push_back(std::stod(cells[6]));
        } catch (const std::invalid_argument& e) {
            logger.error(std::source_location::current(),
                         "Skipping line {} (bad number format): '{}'",
                         line_num,
                         line);
        } catch (const std::out_of_range& e) {
            logger.error(std::source_location::current(),
                         "Skipping line {} (number out of range): '{}'",
                         line_num,
                         line);
        }
    }

    system.positions.shrink_to_fit();
    system.velocities.shrink_to_fit();
    system.masses.shrink_to_fit();

    logger.info(std::source_location::current(),
                "Finished stream generation. Successfully loaded {} particles.",
                system.positions.size());

    return system;
}

}  // namespace enkas::generation
