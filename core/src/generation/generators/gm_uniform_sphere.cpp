#include <random>
#include <vector>
#include <utils.h>

#include "gm_uniform_sphere.h"
#include "geometric_algebra.h"

GM_UniformSphere::GM_UniformSphere(const Settings& settings, unsigned int seed)
    : settings(settings)
    , seed(seed)
{}

utils::InitialSystem GM_UniformSphere::createSystem()
{
    utils::InitialSystem initial_system;
    initial_system.reserve(settings.N);

    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> pos_dist(-settings.radius, settings.radius);
    std::uniform_real_distribution<double> vel_dist(0.0, 1.0);

    for (size_t i = 0; i < settings.N; i++) {
        utils::BaseParticle particle;

        // POSITION
        ga::Vector3D position;

        // Use a rejection technique to carve out a homogeneous sphere from a homogeneous cube.
        do {
            position = ga::Vector3D(pos_dist(gen), pos_dist(gen), pos_dist(gen));
        } while (position.norm() > settings.radius);

        particle.pos = position;

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
