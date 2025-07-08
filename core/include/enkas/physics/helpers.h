#pragma once

#include <vector>
#include <numeric>
#include <cmath>

#include <enkas/data/system.h>
#include <enkas/math/vector3d.h>
#include <enkas/math/bivector3d.h>

namespace enkas::physics {

// Gravitational constant in pc, solar masses and km/s
const double G = 0.004300917271;

/**
 * @brief Calculates the total kinetic energy of a system.
 * @param system The system containing the particles.
 * @return The total kinetic energy of the system.
 */
[[nodiscard]] inline double getKineticEnergy(const data::System& system)
{
    const size_t particle_count = system.count();
    if (particle_count == 0) return 0.0;

    double kinetic_energy = 0.0;

    for (size_t i = 0; i < system.size(); ++i) {
        kinetic_energy += system.masses[i]*system.velocities[i].norm2();
    }

    return kinetic_energy*0.5;
}

/**
 * @brief Calculates the total potential energy of a system in Hénon units.
 * @param system The system containing the particles.
 * @param softening_parameter The softening parameter to avoid singularities.
 * @return The total potential energy of the system.
 * @warning This function assumes Hénon units for the system, which means
 *          that the gravitational constant G is set to 1.
 */
[[nodiscard]] inline double getPotentialEnergy(const data::System& system, double softening_parameter) const
{
    const size_t particle_count = system.count();
    if (particle_count == 0) return 0.0;

    double potential_energy = 0.0;

    // Sum pair-wise potential energy
    for (size_t i = 0; i < particle_count; i++) {
        for (size_t j = i + 1; j < particle_count; j++) {
            const math::Vector3D r_ij = system.positions[j] - system.positions[i];
            const double dist_sqr = r_ij.norm2() + softening_parameter*softening_parameter;
            const double dist_inv = 1.0/std::sqrt(dist_sqr);
            potential_energy -= system.masses[i]*system.masses[j]*dist_inv;
        }
    }

    return potential_energy;
}

/**
 * @brief Calculates the total angular momentum of a system.
 * @param system The system containing the particles.
 * @return The total angular momentum as a Bivector3D.
 */
[[nodiscard]] inline math::Bivector3D getAngularMomentum(const data::System& system)
{
    math::Bivector3D total_angular_momentum{};

    for (size_t i = 0; i < system.size(); ++i) {
        const auto& pos = system.positions[i];
        const auto& vel = system.velocities[i];
        const auto mass = system.masses[i];
        
        total_angular_momentum += math::wedge(pos, vel*mass);
    }

    return total_angular_momentum;
}

/**
 * @brief Translates a system so its center of mass is at the origin (0,0,0)
 *        and its total momentum is zero.
 * @param system The system containing the particles to be centered.
 */
inline void centerSystem(data::System& system)
{
    if (system.size() == 0) {
        return;
    }

    math::Vector3D weighted_pos_sum{};
    math::Vector3D weighted_vel_sum{};
    double total_mass = 0.0;

    for (size_t i = 0; i < system.size(); ++i) {
        const double mass = system.masses[i];
        weighted_pos_sum += system.positions[i]*mass;
        weighted_vel_sum += system.velocities[i]*mass;
        total_mass += mass;
    }

    if (total_mass == 0.0) {
        return;
    }

    const math::Vector3D com_pos = weighted_pos_sum / total_mass;
    const math::Vector3D com_vel = weighted_vel_sum / total_mass;

    for (size_t i = 0; i < system.size(); ++i) {
        system.positions[i] -= com_pos;
        system.velocities[i] -= com_vel;
    }
}

/**
 * @brief Scale the properties of particles to Hénon units.
 *
 * This function scales the properties of a vector of particles, including their
 * masses, positions, and velocities to Hénon units.
 *
 * @param system The system containing the particles to be scaled.
 * @param total_energy The absolute total energy of the system in units of the provided
 *                     particles.
 * 
 * @see Heggie, D. and Methieu, R.; 1986; Standardised units and time scales
 */
inline void scaleToHenonUnits(data::System& system, double total_energy)
{
    if (system.size() == 0) {
        return;
    }

    const double total_mass = std::accumulate(system.masses.begin(), system.masses.end(), 0.0);

    const double mass_unit = total_mass;
    const double length_unit = G*std::pow(total_mass, 2)/(4.0*total_energy);
    const double time_unit = G*std::sqrt(std::pow(total_mass, 5)/std::pow(4.0*total_energy, 3));
    const double velocity_unit = length_unit/time_unit;

    for (auto& mass : system.masses) {
        mass /= mass_unit;
    }

    for (auto& position : system.positions) {
        position /= length_unit;
    }

    for (auto& velocity : system.velocities) {
        velocity /= velocity_unit;
    }
}

} // namespace enkas::physics
