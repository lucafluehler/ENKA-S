#include <enkas/data/system.h>
#include <enkas/generation/generators/stream_generator.h>
#include <enkas/math/vector3d.h>

#include <istream>
#include <sstream>
#include <string>
#include <vector>

namespace enkas::generation {

StreamGenerator::StreamGenerator(std::istream& stream) : stream_(stream) {}

data::System StreamGenerator::createSystem() {
    data::System system;
    std::string line;

    if (!stream_) {
        return system;
    }

    // Skip header
    std::getline(stream_, line);

    // Load particles one by one :)
    while (std::getline(stream_, line)) {
        std::stringstream line_stream(line);
        std::string cell;
        std::vector<std::string> cells;

        // Split the line by commas into a vector of strings.
        while (std::getline(line_stream, cell, ',')) {
            cells.push_back(cell);
        }

        if (cells.size() >= 7) {
            try {
                math::Vector3D position = {
                    std::stod(cells[0]), std::stod(cells[1]), std::stod(cells[2])};
                math::Vector3D velocity = {
                    std::stod(cells[3]), std::stod(cells[4]), std::stod(cells[5])};

                system.positions.push_back(position);
                system.velocities.push_back(velocity);
                system.masses.push_back(std::stod(cells[6]));
            } catch (const std::invalid_argument& e) {
                // This line could not be parsed as numbers.
            } catch (const std::out_of_range& e) {
                // A number was too large to fit in a double. Skip line.
            }
        }
    }

    system.positions.shrink_to_fit();
    system.velocities.shrink_to_fit();
    system.masses.shrink_to_fit();

    return system;
}

}  // namespace enkas::generation
