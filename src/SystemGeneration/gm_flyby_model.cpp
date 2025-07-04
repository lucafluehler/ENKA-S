#include <random>
#include <vector>

#include "gm_flyby_model.h"
#include "gm_plummer_sphere.h"
#include "utils.h"

GM_FlybyModel::GM_FlybyModel(const Settings& settings, unsigned int seed)
    : settings(settings)
    , seed(seed)
{}

utils::InitialSystem GM_FlybyModel::createSystem()
{
    utils::InitialSystem initial_system;
    initial_system.reserve(settings.N + 1); // add one for massive object

    // Generate a Plummer sphere
    auto plummer_settings = GM_PlummerSphere::Settings();
    plummer_settings.N = settings.N;
    plummer_settings.radius = settings.radius;
    plummer_settings.total_mass = settings.total_mass;

    initial_system = GM_PlummerSphere(plummer_settings, seed).createSystem();

    // Add a massive object drifting towards the sphere
    const double c_DISTANCE = 6*std::pow(settings.radius, 1.0/3.0);

    utils::BaseParticle massive_particle;
    massive_particle.mass = settings.body_mass;
    massive_particle.pos = ga::Vector3D(c_DISTANCE, 3*settings.radius, 0.0);
    massive_particle.vel = ga::Vector3D(-4.0, 0.0, 0.0);
    initial_system.push_back(massive_particle);

    centerParticles(initial_system);

    return initial_system;
}
