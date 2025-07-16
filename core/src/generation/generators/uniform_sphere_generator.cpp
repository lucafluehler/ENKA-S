#include <enkas/data/system.h>
#include <enkas/generation/generators/uniform_sphere_generator.h>
#include <enkas/logging/logger.h>
#include <enkas/math/vector3d.h>
#include <enkas/physics/helpers.h>

#include <random>
#include <vector>

namespace enkas::generation {

UniformSphereGenerator::UniformSphereGenerator(const UniformSphereSettings& settings)
    : settings_(settings) {}

data::System UniformSphereGenerator::createSystem() {
    ENKAS_LOG_INFO("Creating 'UniformSphere' system...");

    data::System system;
    const int particle_count = settings_.particle_count;

    system.positions.reserve(particle_count);
    system.velocities.reserve(particle_count);
    system.masses.reserve(particle_count);

    std::mt19937 gen(settings_.seed);
    std::uniform_real_distribution<double> pos_dist(-settings_.sphere_radius,
                                                    settings_.sphere_radius);
    std::uniform_real_distribution<double> vel_dist(0.0, 1.0);

    const double particle_mass = settings_.total_mass / particle_count;

    for (size_t i = 0; i < particle_count; i++) {
        math::Vector3D position;

        // Use a rejection technique to carve out a homogeneous sphere from a homogeneous cube.
        do {
            position = {pos_dist(gen), pos_dist(gen), pos_dist(gen)};
        } while (position.norm() > settings_.sphere_radius);

        math::Vector3D velocity = {vel_dist(gen), vel_dist(gen), vel_dist(gen)};
        velocity.set_norm(settings_.initial_velocity);

        system.positions.push_back(position);
        system.velocities.push_back(velocity);
        system.masses.push_back(particle_mass);
    }

    physics::centerSystem(system);

    ENKAS_LOG_INFO("Finished 'UniformSphere' generation. Successfully loaded {} particles.",
                   system.positions.size());

    return system;
}

}  // namespace enkas::generation
