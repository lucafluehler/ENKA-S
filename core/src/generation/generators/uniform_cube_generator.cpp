#include <random>
#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generators/uniform_cube_generator.h>
#include <enkas/physics/physics_helpers.h>

namespace enkas::generation {

UniformCubeGenerator::UniformCubeGenerator(const UniformCubeSettings& settings, unsigned int seed)
    : settings(settings)
    , seed(seed)
{}

data::InitialSystem UniformCubeGenerator::createSystem()
{
    data::InitialSystem initial_system;
    initial_system.reserve(settings.N);

    std::mt19937 gen(seed);

    const double c = settings.side_length/2.0;
    std::uniform_real_distribution<double> pos_dist(-c, c);
    std::uniform_real_distribution<double> vel_dist(0.0, 1.0);

    for (size_t i = 0; i < settings.N; i++) {
        data::BaseParticle particle;

        // POSITION
        particle.pos = math::Vector3D(pos_dist(gen), pos_dist(gen), pos_dist(gen));

        // VELOCITY
        particle.vel = math::Vector3D(vel_dist(gen), vel_dist(gen), vel_dist(gen));
        particle.vel.set_norm(settings.vel);

        // MASS
        particle.mass = settings.total_mass/settings.N;

        initial_system.push_back(particle);
    }

    physics::centerParticles(initial_system);

    return initial_system;
}

} // namespace enkas::generation
