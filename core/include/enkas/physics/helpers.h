#pragma once

#include <enkas/data/system.h>
#include <enkas/math/bivector3d.h>
#include <enkas/math/vector3d.h>

#include <cmath>
#include <numeric>
#include <vector>

namespace enkas::physics {

// Gravitational constant in pc, solar masses and km/s
const double G = 0.004300917271;

/**
 * @brief Calculates the total kinetic energy of a system.
 * @param system The system containing the particles.
 * @return The total kinetic energy of the system.
 */
[[nodiscard]]
inline double getKineticEnergy(const data::System& system) noexcept {
    const size_t particle_count = system.count();
    if (particle_count == 0) return 0.0;

    const auto& masses = system.masses;
    const auto& velocities = system.velocities;

    double kinetic_energy = 0.0;

    for (size_t i = 0; i < system.count(); ++i) {
        kinetic_energy += masses[i] * velocities[i].norm2();
    }

    return kinetic_energy * 0.5;
}

/**
 * @brief Calculates the total potential energy of a system in Hénon units.
 * @param system The system containing the particles.
 * @param softening_parameter The softening parameter to avoid singularities. If
 * set to zero, values like NaN or Inf may occur.
 * @return The total potential energy of the system.
 * @warning This function assumes Hénon units for the system, which means
 *          that the gravitational constant G is set to 1.
 */
[[nodiscard]]
inline double getPotentialEnergy(const data::System& system, double softening_parameter) noexcept {
    const size_t particle_count = system.count();
    if (particle_count == 0) return 0.0;

    const auto& masses = system.masses;
    const auto& positions = system.positions;
    const double softening_sqr = softening_parameter * softening_parameter;

    double potential_energy = 0.0;

    for (size_t i = 0; i < particle_count; i++) {
        for (size_t j = i + 1; j < particle_count; j++) {
            const math::Vector3D r_ij = positions[j] - positions[i];

            const double dist_sqr = r_ij.norm2() + softening_sqr;
            const double dist_inv = 1.0 / std::sqrt(dist_sqr);

            potential_energy -= masses[i] * masses[j] * dist_inv;
        }
    }

    return potential_energy;
}

/**
 * @brief Calculates the total angular momentum of a system.
 * @param system The system containing the particles.
 * @return The total angular momentum as a Bivector3D.
 */
[[nodiscard]]
inline math::Bivector3D getAngularMomentum(const data::System& system) {
    math::Bivector3D total_angular_momentum;

    const size_t particle_count = system.count();
    if (particle_count == 0) return total_angular_momentum;

    for (size_t i = 0; i < particle_count; ++i) {
        const auto& pos = system.positions[i];
        const auto& vel = system.velocities[i];
        const double mass = system.masses[i];

        total_angular_momentum += math::wedge(pos, vel * mass);
    }

    return total_angular_momentum;
}

/**
 * @brief Represents the center of mass of a system
 *        with its position and velocity.
 */
struct CenterOfMass {
    math::Vector3D position;
    math::Vector3D velocity;
};

/**
 * @brief Calculates the center of mass position and velocity for a system.
 * @param system The system to analyze.
 * @return A CenterOfMass struct containing the calculated properties.
 *         Returns a zeroed struct if the total mass is zero.
 */
[[nodiscard]] inline CenterOfMass getCenterOfMass(const data::System& system) {
    const size_t particle_count = system.count();
    if (particle_count == 0) return {};

    math::Vector3D weighted_pos_sum;
    math::Vector3D weighted_vel_sum;
    double total_mass = 0.0;

    for (size_t i = 0; i < particle_count; ++i) {
        const double mass = system.masses[i];
        weighted_pos_sum += system.positions[i] * mass;
        weighted_vel_sum += system.velocities[i] * mass;
        total_mass += mass;
    }

    if (total_mass == 0.0) return {};

    CenterOfMass com;
    com.position = weighted_pos_sum / total_mass;
    com.velocity = weighted_vel_sum / total_mass;

    return com;
}

/**
 * @brief Translates a system so its center of mass is at the origin (0,0,0)
 *        and its total momentum is zero.
 * @param system The system containing the particles to be centered.
 */
inline void centerSystem(data::System& system) {
    const size_t particle_count = system.count();
    if (particle_count == 0) return;

    const CenterOfMass com = getCenterOfMass(system);

    for (size_t i = 0; i < particle_count; ++i) {
        system.positions[i] -= com.position;
        system.velocities[i] -= com.velocity;
    }
}

/**
 * @brief Scale the properties of particles to Hénon units.
 *
 * This function scales the properties of a vector of particles, including their
 * masses, positions, and velocities to Hénon units.
 *
 * @param system The system containing the particles to be scaled.
 * @param total_energy The absolute total energy of the system in units of the
 * provided particles.
 *
 * @see Heggie, D. and Methieu, R.; 1986; Standardised units and time scales
 */
inline void scaleToHenonUnits(data::System& system, double total_energy) {
    if (system.count() == 0) return;

    const double total_mass = std::accumulate(system.masses.begin(), system.masses.end(), 0.0);

    const double mass_unit = total_mass;
    const double length_unit = G * std::pow(total_mass, 2) / (4.0 * total_energy);
    const double time_unit =
        G * std::sqrt(std::pow(total_mass, 5) / std::pow(4.0 * total_energy, 3));
    const double velocity_unit = length_unit / time_unit;

    for (auto& m : system.masses) m /= mass_unit;
    for (auto& p : system.positions) p /= length_unit;
    for (auto& v : system.velocities) v /= velocity_unit;
}

}  // namespace enkas::physics
