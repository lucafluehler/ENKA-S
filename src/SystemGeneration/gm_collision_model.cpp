#include <random>
#include <vector>

#include "utils.h"
#include "gm_collision_model.h"
#include "gm_plummer_sphere.h"

GM_CollisionModel::GM_CollisionModel(const Settings& settings, unsigned int seed)
    : settings(settings)
    , seed(seed)
{}

utils::InitialSystem GM_CollisionModel::createSystem()
{
    const double c_AVG_RADIUS = (settings.radius_1 + settings.radius_2)/2.0;
    const double c_HALF_DISTANCE = std::pow(c_AVG_RADIUS, 1.0/3.0)*1.2;

    // Generate first Plummer sphere
    auto plummer_1_settings = GM_PlummerSphere::Settings();
    plummer_1_settings.N = settings.N_1;
    plummer_1_settings.radius = settings.radius_1;
    plummer_1_settings.total_mass = settings.total_mass_1;

    auto sphere_1 = GM_PlummerSphere(plummer_1_settings, seed).createSystem();

    centerParticles(sphere_1);

    // Move particles in positive x_direction and add drift in opposite direction
    for (auto& particle: sphere_1) {
        particle.pos.x += c_HALF_DISTANCE;
        particle.pos.y += settings.radius_1;
        particle.vel.x -= 4.0;
    }

    // Generate second centered Plummer sphere
    auto plummer_2_settings = GM_PlummerSphere::Settings();
    plummer_2_settings.N = settings.N_2;
    plummer_2_settings.radius = settings.radius_2;
    plummer_2_settings.total_mass = settings.total_mass_2;

    auto sphere_2 = GM_PlummerSphere(plummer_2_settings, seed).createSystem();

    centerParticles(sphere_2);

    // Move particles in negative x_direction and add drift in opposite direction
    for (auto& particle: sphere_2) {
        particle.pos.x -= c_HALF_DISTANCE;
        particle.pos.y -= settings.radius_2;
        particle.vel.x += 4.0;
    }

    // Combine both systems and center
    utils::InitialSystem initial_system;
    initial_system.reserve(settings.N_1 + settings.N_2);

    initial_system.insert(initial_system.end(), sphere_1.begin(), sphere_1.end());
    initial_system.insert(initial_system.end(), sphere_2.begin(), sphere_2.end());

    centerParticles(initial_system);

    return initial_system;
}
