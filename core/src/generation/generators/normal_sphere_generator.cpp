#include <random>
#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generators/normal_sphere_generator.h>
#include <enkas/physics/physics_helpers.h>

namespace enkas::generation {

NormalSphereGenerator::NormalSphereGenerator(const NormalSphereSettings& settings, unsigned int seed)
    : settings(settings)
    , seed(seed)
{}

InitialSystem NormalSphereGenerator::createSystem()
{
    InitialSystem initial_system;
    initial_system.reserve(settings.N);

    std::mt19937 gen(seed);

    std::normal_distribution<double> pos_dist(0, settings.pos_std);
    std::normal_distribution<double> vel_dist(0, settings.vel_std);
    std::normal_distribution<double> mass_dist(settings.mass_mean, settings.mass_std);

    for (size_t i = 0; i < settings.N; i++) {
        data::BaseParticle particle;

        particle.pos = math::Vector3D(pos_dist(gen), pos_dist(gen), pos_dist(gen));
        particle.vel = math::Vector3D(vel_dist(gen), vel_dist(gen), vel_dist(gen));
        particle.mass = std::abs(mass_dist(gen));

        initial_system.push_back(particle);
    }

    physics::centerParticles(initial_system);

    return initial_system;
}

} // namespace enkas::generation
