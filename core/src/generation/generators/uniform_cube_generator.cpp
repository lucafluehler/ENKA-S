#include <enkas/data/system.h>
#include <enkas/generation/generators/uniform_cube_generator.h>
#include <enkas/logging/logger.h>
#include <enkas/math/vector3d.h>
#include <enkas/physics/helpers.h>

#include <random>
#include <vector>


namespace enkas::generation {

UniformCubeGenerator::UniformCubeGenerator(const UniformCubeSettings& settings, unsigned int seed)
    : settings_(settings), seed_(seed) {}

data::System UniformCubeGenerator::createSystem() {
    auto& logger = logging::getLogger();
    logger.info("Creating 'UniformCube' system...");

    data::System system;
    const int particle_count = settings_.particle_count;

    system.positions.reserve(particle_count);
    system.velocities.reserve(particle_count);
    system.masses.reserve(particle_count);

    std::mt19937 gen(seed_);

    const double half_side = settings_.side_length / 2.0;
    std::uniform_real_distribution<double> pos_dist(-half_side, half_side);
    std::uniform_real_distribution<double> vel_dist(0.0, 1.0);

    const double particle_mass = settings_.total_mass / particle_count;

    for (size_t i = 0; i < particle_count; i++) {
        math::Vector3D position = {pos_dist(gen), pos_dist(gen), pos_dist(gen)};
        math::Vector3D velocity = {vel_dist(gen), vel_dist(gen), vel_dist(gen)};
        velocity.set_norm(settings_.initial_velocity);
        system.positions.push_back(position);
        system.velocities.push_back(velocity);
        system.masses.push_back(particle_mass);
    }

    physics::centerSystem(system);

    logger.info("Finished 'UniformCube' generation. Successfully loaded {} particles.",
                system.positions.size());

    return system;
}

}  // namespace enkas::generation
