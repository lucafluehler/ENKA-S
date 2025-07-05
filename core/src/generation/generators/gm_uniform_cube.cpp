#include <random>
#include <vector>
#include <utils.h>

#include "gm_uniform_cube.h"
#include "geometric_algebra.h"

GM_UniformCube::GM_UniformCube(const Settings& settings, unsigned int seed)
    : settings(settings)
    , seed(seed)
{}

utils::InitialSystem GM_UniformCube::createSystem()
{
    utils::InitialSystem initial_system;
    initial_system.reserve(settings.N);

    std::mt19937 gen(seed);

    const double c = settings.side_length/2.0;
    std::uniform_real_distribution<double> pos_dist(-c, c);
    std::uniform_real_distribution<double> vel_dist(0.0, 1.0);

    for (size_t i = 0; i < settings.N; i++) {
        utils::BaseParticle particle;

        // POSITION
        particle.pos = ga::Vector3D(pos_dist(gen), pos_dist(gen), pos_dist(gen));

        // VELOCITY
        particle.vel = ga::Vector3D(vel_dist(gen), vel_dist(gen), vel_dist(gen));
        particle.vel.set_norm(settings.vel);

        // MASS
        particle.mass = settings.total_mass/settings.N;

        initial_system.push_back(particle);
    }

    centerParticles(initial_system);

    return initial_system;
}
