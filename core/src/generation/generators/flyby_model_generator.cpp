#include <random>
#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generators/flyby_model_generator.h>
#include <enkas/generation/settings/plummer_sphere_generator.h>
#include <enkas/physics/physics_helpers.h>

namespace enkas {
namespace generation {

FlybyModelGenerator::FlybyModelGenerator(const FlybyModelSettings& settings, unsigned int seed)
    : settings(settings)
    , seed(seed)
{}

data::InitialSystem FlybyModelGenerator::createSystem()
{
    data::InitialSystem initial_system;
    initial_system.reserve(settings.N + 1); // add one for massive object

    // Generate a Plummer sphere
    auto plummer_settings = PlummerSphereSettings();
    plummer_settings.N = settings.N;
    plummer_settings.radius = settings.radius;
    plummer_settings.total_mass = settings.total_mass;

    initial_system = PlummerSphereGenerator(plummer_settings, seed).createSystem();

    // Add a massive object drifting towards the sphere
    const double c_DISTANCE = 6*std::pow(settings.radius, 1.0/3.0);

    utils::BaseParticle massive_particle;
    massive_particle.mass = settings.body_mass;
    massive_particle.pos = math::Vector3D(c_DISTANCE, 3*settings.radius, 0.0);
    massive_particle.vel = math::Vector3D(-4.0, 0.0, 0.0);
    initial_system.push_back(massive_particle);

    physics::centerParticles(initial_system);

    return initial_system;
}

} // namespace generation
} // namespace enkas
