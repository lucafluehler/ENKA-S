#include <enkas/data/system.h>
#include <enkas/generation/generators/spiral_galaxy_generator.h>
#include <enkas/logging/logger.h>
#include <enkas/math/helpers.h>
#include <enkas/math/vector3d.h>
#include <enkas/physics/helpers.h>

#include <numbers>
#include <random>
#include <vector>

namespace enkas::generation {

SpiralGalaxyGenerator::SpiralGalaxyGenerator(const SpiralGalaxySettings& settings,
                                             unsigned int seed)
    : settings_(settings), seed_(seed) {}

data::System SpiralGalaxyGenerator::createSystem() {
    ENKAS_LOG_INFO("Creating 'SpiralGalaxy' system...");

    data::System system;
    const int particle_count = settings_.particle_count;

    // Additional particle for the black hole
    system.positions.reserve(particle_count + 1);
    system.velocities.reserve(particle_count + 1);
    system.masses.reserve(particle_count + 1);

    const double stellar_mass = settings_.total_mass / particle_count;
    const double inner_radius = settings_.radius / 40.0;

    std::mt19937 gen(seed_);
    std::normal_distribution<double> disk_thickness_dist(0.0, settings_.radius / 100.0);

    // Generate Disk
    const int num_particles_per_arm = particle_count / settings_.num_arms;

    for (size_t k = 0; k < settings_.num_arms; k++) {
        for (size_t i = 0; i < num_particles_per_arm; i++) {
            const double distance = inner_radius + settings_.radius * i / settings_.particle_count;
            const double angle = (settings_.twist * std::numbers::pi * i / num_particles_per_arm) +
                                 (2 * std::numbers::pi * k / settings_.num_arms);

            math::Vector3D position = {std::sin(angle), std::cos(angle), 0.0};
            position.set_norm(distance);

            // This can also be done with a thicc GA statement using Vectors, Bivectors
            // and Rotors
            // particle.pos = math::Rotor3D(angle, math::Bivector3D::XY()).normalize()
            //                .rotate(math::Vector3D::X(distance));

            const double eccentricity_mean =
                0.4 / (1 + std::exp((particle_count / 50.0 - i) / 4.0)) + 0.05;
            std::normal_distribution<double> eccentricity_dist(eccentricity_mean, 0.1);
            double eccentricity = eccentricity_dist(gen);

            // rejection technique to ensure an elliptic trajectory
            while (eccentricity >= 1.0 || eccentricity <= 0.0) {
                eccentricity = eccentricity_dist(gen);
            }

            const double major_half_axis = distance / (1 + eccentricity);
            const double first_term =
                physics::G * (settings_.black_hole_mass + settings_.total_mass);
            const double second_term = (2.0 / distance - 1.0 / major_half_axis);
            const double speed = std::sqrt(first_term * second_term);
            math::Vector3D velocity = {position.y, -position.x, 0.0};
            velocity.set_norm(-speed);

            // This can also be done using a GA statement by taking the Hodge Dual of the
            // Bivector spanned by wedging the position vector with the unit
            // z-axis vector... which is the same as the cross product *yuck*
            // particle.vel = math::wedge(particle.pos, math::Vector3D::Z()).getPerpendicular()
            //               .set_norm(c_VELOCITY)*(-1);

            position.z = disk_thickness_dist(gen);

            system.positions.push_back(position);
            system.velocities.push_back(velocity);
            system.masses.push_back(stellar_mass);
        }
    }

    physics::centerSystem(system);

    // Add black hole to center
    system.positions.push_back(math::Vector3D{});
    system.velocities.push_back(math::Vector3D{});
    system.masses.push_back(settings_.black_hole_mass);

    ENKAS_LOG_INFO("Finished 'SpiralGalaxy' generation. Successfully loaded {} particles.",
                   system.positions.size());

    return system;
}

}  // namespace enkas::generation
