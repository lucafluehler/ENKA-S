#include <random>
#include <vector>

#include <enkas/data/systems.h>
#include <enkas/math/vector3d.h>
#include <enkas/generation/generators/collision_model_generator.h>
#include <enkas/generation/generators/plummer_sphere_generator.h>
#include <enkas/physics/helpers.h>

namespace enkas::generation {

CollisionModelGenerator::CollisionModelGenerator(const CollisionModelSettings& settings, unsigned int seed)
    : settings_(settings)
    , seed_(seed)
{}

data::System CollisionModelGenerator::createSystem()
{
    // Generate first Plummer sphere
    PlummerSphereSettings plummer1_settings;
    plummer1_settings.particle_count = m_settings.particle_count_1;
    plummer1_settings.sphere_radius  = m_settings.radius_1;
    plummer1_settings.total_mass     = m_settings.total_mass_1;

    data::System sphere1 = PlummerSphereGenerator(plummer1_settings, m_seed).createSystem();

    // Generate second Plummer sphere
    PlummerSphereSettings plummer2_settings;
    plummer2_settings.particle_count = m_settings.particle_count_2;
    plummer2_settings.sphere_radius  = m_settings.radius_2;
    plummer2_settings.total_mass     = m_settings.total_mass_2;

    data::System sphere2 = PlummerSphereGenerator(plummer2_settings, m_seed + 1).createSystem();

    const double avg_radius = (settings_.sphere_radius_1 + settings_.sphere_radius_2)/2.0;
    const double separation_distance = settings_.impact_parameter;

    // Move first sphere
    for (auto& pos : sphere1.positions) {
        pos.x += separation_distance/2.0;
    }

    for (auto& vel : sphere1.velocities) {
        vel.x += settings_.relative_velocity/2.0;
    }

    // Move second sphere
    for (auto& pos : sphere2.positions) {
        pos.x -= separation_distance/2.0;
    }

    for (auto& vel : sphere2.velocities) {
        vel.x -= settings_.relative_velocity/2.0;
    }

    // Combine both systems and center
    data::System system;
    const size_t total_particles = sphere1.count() + sphere2.count();

    system.positions.reserve(total_particles);
    system.velocities.reserve(total_particles);
    system.masses.reserve(total_particles);

    system.positions.insert(
        system.positions.end(), 
        std::make_move_iterator(sphere1.positions.begin()), 
        std::make_move_iterator(sphere1.positions.end())
    );
    system.velocities.insert(
        system.velocities.end(), 
        std::make_move_iterator(sphere1.velocities.begin()), 
        std::make_move_iterator(sphere1.velocities.end())
    );
    system.masses.insert(
        system.masses.end(), 
        std::make_move_iterator(sphere1.masses.begin()), 
        std::make_move_iterator(sphere1.masses.end())
    );

    system.positions.insert(
        system.positions.end(), 
        std::make_move_iterator(sphere2.positions.begin()), 
        std::make_move_iterator(sphere2.positions.end())
    );
    system.velocities.insert(
        system.velocities.end(), 
        std::make_move_iterator(sphere2.velocities.begin()), 
        std::make_move_iterator(sphere2.velocities.end())
    );
    system.masses.insert(
        system.masses.end(), 
        std::make_move_iterator(sphere2.masses.begin()), 
        std::make_move_iterator(sphere2.masses.end())
    );

    physics::centerSystem(system);

    return system;
}

} // namespace enkas::generation
