#include <random>
#include <vector>

#include "gm_normal_sphere.h"
#include "utils.h"

GM_NormalSphere::GM_NormalSphere(const Settings& settings, unsigned int seed)
    : settings(settings)
    , seed(seed)
{}

utils::InitialSystem GM_NormalSphere::createSystem()
{
    utils::InitialSystem initial_system;
    initial_system.reserve(settings.N);

    std::mt19937 gen(seed);

    std::normal_distribution<double> pos_dist(0, settings.pos_std);
    std::normal_distribution<double> vel_dist(0, settings.vel_std);
    std::normal_distribution<double> mass_dist(settings.mass_mean, settings.mass_std);

    for (size_t i = 0; i < settings.N; i++) {
        utils::BaseParticle particle;

        // POSITION
        particle.pos = ga::Vector3D(pos_dist(gen), pos_dist(gen), pos_dist(gen));

        // VELOCITY
        particle.vel = ga::Vector3D(vel_dist(gen), vel_dist(gen), vel_dist(gen));
        
        // MASS
        particle.mass = mass_dist(gen);

        initial_system.push_back(particle);
    }

    centerParticles(initial_system);

    return initial_system;
}
