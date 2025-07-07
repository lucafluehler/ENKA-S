#include <random>
#include <vector>

#include <enkas/data/system.h>
#include <enkas/math/vector3d.h>
#include <enkas/generation/generators/normal_sphere_generator.h>
#include <enkas/physics/helpers.h>

namespace enkas::generation {

NormalSphereGenerator::NormalSphereGenerator(const NormalSphereSettings& settings, unsigned int seed)
    : settings_(settings)
    , seed_(seed)
{}

data::System NormalSphereGenerator::createSystem()
{
    data::System system;
    const int particle_count = settings_.particle_count;

    system.positions.reserve(particle_count);
    system.velocities.reserve(particle_count);
    system.masses.reserve(particle_count);

    std::mt19937 gen(seed);

    std::normal_distribution<double> pos_dist(0, settings_.position_std_dev);
    std::normal_distribution<double> vel_dist(0, settings_.velocity_std_dev);
    std::normal_distribution<double> mass_dist(settings_.mass_mean, settings_.mass_std);

    for (size_t i = 0; i < particle_count; i++) {
        math::Vector3D position = {pos_dist(gen), pos_dist(gen), pos_dist(gen)};
        math::Vector3D velocity = {vel_dist(gen), vel_dist(gen), vel_dist(gen)};
        double particle_mass = std::abs(mass_dist(gen));

        system.positions.push_back(position);
        system.velocities.push_back(velocity);
        system.masses.push_back(particle_mass);
    }

    physics::centerSystem(system);

    return system;
}

} // namespace enkas::generation
