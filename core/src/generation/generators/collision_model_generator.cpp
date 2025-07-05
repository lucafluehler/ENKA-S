#include <random>
#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generators/collision_model_generator.h>
#include <enkas/generation/generators/plummer_sphere_generator.h>
#include <enkas/physics/physics_helpers.h>

namespace enkas::generation {

CollisionModelGenerator::CollisionModelGenerator(const CollisionModelSettings& settings, unsigned int seed)
    : settings(settings)
    , seed(seed)
{}

data::InitialSystem CollisionModelGenerator::createSystem()
{
    const double c_AVG_RADIUS = (settings.radius_1 + settings.radius_2)/2.0;
    const double c_HALF_DISTANCE = std::pow(c_AVG_RADIUS, 1.0/3.0)*1.2;

    // Generate first Plummer sphere
    auto plummer_1_settings = PlummerSphereSettings();
    plummer_1_settings.N = settings.N_1;
    plummer_1_settings.radius = settings.radius_1;
    plummer_1_settings.total_mass = settings.total_mass_1;

    auto sphere_1 = PlummerSphereGenerator(plummer_1_settings, seed).createSystem();

    physics::centerParticles(sphere_1);

    // Move particles in positive x_direction and add drift in opposite direction
    for (auto& particle: sphere_1) {
        particle.pos.x += c_HALF_DISTANCE;
        particle.pos.y += settings.radius_1;
        particle.vel.x -= 4.0;
    }

    // Generate second centered Plummer sphere
    auto plummer_2_settings = PlummerSphereSettings();
    plummer_2_settings.N = settings.N_2;
    plummer_2_settings.radius = settings.radius_2;
    plummer_2_settings.total_mass = settings.total_mass_2;

    auto sphere_2 = PlummerSphereGenerator(plummer_2_settings, seed).createSystem();

    physics::centerParticles(sphere_2);

    // Move particles in negative x_direction and add drift in opposite direction
    for (auto& particle: sphere_2) {
        particle.pos.x -= c_HALF_DISTANCE;
        particle.pos.y -= settings.radius_2;
        particle.vel.x += 4.0;
    }

    // Combine both systems and center
    data::InitialSystem initial_system;
    initial_system.reserve(settings.N_1 + settings.N_2);

    initial_system.insert(initial_system.end(), sphere_1.begin(), sphere_1.end());
    initial_system.insert(initial_system.end(), sphere_2.begin(), sphere_2.end());

    physics::centerParticles(initial_system);

    return initial_system;
}

} // namespace enkas::generation
