#include <random>
#include <vector>

#include <enkas/data/system.h>
#include <enkas/math/vector3d.h>
#include <enkas/math/helpers.h>
#include <enkas/generation/generators/plummer_sphere_generator.h>
#include <enkas/physics/helpers.h>

namespace enkas::generation {

PlummerSphereGenerator::PlummerSphereGenerator(const PlummerSphereSettings& settings, unsigned int seed)
    : settings_(settings)
    , seed_(seed)
{}

data::System PlummerSphereGenerator::createSystem()
{
    data::System system;
    const int particle_count = settings_.particle_count;

    system.positions.reserve(particle_count);
    system.velocities.reserve(particle_count);
    system.masses.reserve(particle_count);

    const double plummer_radius = settings_.sphere_radius;
    const double particle_mass = settings_.total_mass/particle_count;

    std::mt19937 gen(seed_);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (size_t i = 0; i < particle_count; i++) {
        // POSITION (Aarseth, S. J. 2003, Gravitational N-Body Simulations)
        // This method generates a radius 'r' based on the mass distribution.
        const double m_i = (i + 1.0)/particle_count; // Cumulative mass fraction
        const double r = plummer_radius/std::sqrt(std::pow(m_i, -2.0/3.0) - 1.0);
        
        // Use the new, namespaced helper function to get a random direction.
        const math::Vector3D position = math::getRandOnSphere(gen, r);

        // VELOCITY (Rejection sampling method from Aarseth, 2003)
        double q = 0.0;
        double g_q = 0.0;
        do {
            q = dist(gen); // Generate a random value [0, 1] for velocity magnitude
            g_q = dist(gen)*0.1; // Generate a random value [0, 0.1] for comparison
        } while (g_q > q*q*std::pow(1.0 - q*q, 3.5));

        const double escape_velocity = std::sqrt(2.0*physics::G*settings_.total_mass)* 
                                       std::pow(plummer_radius*plummer_radius + r*r, -0.25);
        
        const double speed = q*escape_velocity;

        // Use the new helper function again for the velocity direction.
        const math::Vector3D velocity = math::getRandOnSphere(gen, speed);

        // --- Push properties to their SoA vectors ---
        system.positions.push_back(position);
        system.velocities.push_back(velocity);
        system.masses.push_back(particle_mass);
    }

    physics::centerSystem(system);

    return system;
}

} // namespace enkas::generation
