#include <string>
#include <vector>

#include <enkas/generation/stream_generator.h>
#include <enkas/data/initial_system.h>

namespace enkas {
namespace generation {

StreamGenerator::StreamGenerator(std::istream& stream)
    : stream(stream)
{}

data::InitialSystem StreamGenerator::createSystem()
{
    data::InitialSystem initial_system;
    std::string line;
    
    if (!stream) {
        return initial_system;
    }

    // Skip header
    std::getline(stream, line);

    // Load particles one by one :)
    while (std::getline(stream, line)) {
        std::stringstream line_stream(line);
        std::string cell;
        std::vector<std::string> cells;

        // Split the line by commas into a vector of strings.
        while (std::getline(line_stream, cell, ',')) {
            cells.push_back(cell);
        }

        if (cells.size() >= 7) {
            try {
                data::BaseParticle particle;
                particle.pos.x = std::stod(cells[0]);
                particle.pos.y = std::stod(cells[1]);
                particle.pos.z = std::stod(cells[2]);
                particle.vel.x = std::stod(cells[3]);
                particle.vel.y = std::stod(cells[4]);
                particle.vel.z = std::stod(cells[5]);
                particle.mass  = std::stod(cells[6]);

                initial_system.push_back(particle);
            }
            catch (const std::invalid_argument& e) {
                // This line could not be parsed as numbers.
            }
            catch (const std::out_of_range& e) {
                // A number was too large to fit in a double. Skip line.
            }
        }
    }

    initial_system.shrink_to_fit();

    return initial_system;
}

} // namespace generation
} // namespace enkas
