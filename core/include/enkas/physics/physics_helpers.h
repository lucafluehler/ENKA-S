#pragma once

#include <vector>

#include <enkas/data/base_particle.h>
#include <enkas/math/vector3d.h>
#include <enkas/math/bivector3d.h>

namespace enkas::physics {

// Gravitational constant in pc, solar masses and km/s
const double G = 0.004300917271;

/**
 * @brief Calculates the total kinetic energy of a system
 *
 * This function calculates the total kinetic energy of particles in a container,
 * where each particle type must be derived from BaseParticle.
 *
 * @tparam T The particle type.
 * @param particles The container of particles.
 *
 * @return The total kinetic energy.
 */
template <DerivedFromBaseParticle T>
inline double getKineticEnergy(const std::vector<T>& particles)
{
    double e_kin = 0.0;

    for (const auto& particle: particles) {
        e_kin += particle.mass*particle.vel.norm2();
    }

    return e_kin*0.5;
}

/**
 * @brief Calculates the total angular momentum of particles in a container.
 *
 * This function calculates the total angular momentum of particles in a container,
 * where each particle type must be derived from BaseParticle.
 *
 * @tparam T The particle type.
 * @param particles The container of particles.
 *
 * @return The total angular momentum.
 */
template <DerivedFromBaseParticle T>
inline math::Bivector3D getAngularMomentum(const std::vector<T>& particles)
{
    math::Bivector3D L_tot;

    for (const auto& particle: particles) {
        L_tot += math::wedge(particle.pos, particle.vel*particle.mass);
    }

    return L_tot;
}

/**
 * @brief Calculates the center of mass position of particles in a container.
 *
 * This function calculates the center of mass position of particles in a container,
 * where each particle type must be derived from BaseParticle.
 *
 * @tparam T The particle type.
 * @param particles The container of particles.
 *
 * @return The center of mass position.
 */
template <DerivedFromBaseParticle T>
inline math::Vector3D getCenterOfMassPos(const std::vector<T>& particles)
{
    math::Vector3D com_pos;

    double total_mass = 0.0;
    for (const auto& particle: particles) {
        com_pos += particle.pos*particle.mass;
        total_mass += particle.mass;
    }

    return com_pos/total_mass;
}

/**
 * @brief Calculates the center of mass velocity of particles in a container.
 *
 * This function calculates the center of mass velocity of particles in a container,
 * where each particle type must be derived from BaseParticle.
 *
 * @tparam T The particle type.
 * @param particles The container of particles.
 *
 * @return The center of mass velocity.
 */
template <DerivedFromBaseParticle T>
inline math::Vector3D getCenterOfMassVel(const std::vector<T>& particles)
{
    math::Vector3D com_vel;

    double total_mass = 0.0;
    for (const auto& particle: particles) {
        com_vel += particle.vel*particle.mass;
        total_mass += particle.mass;
    }

    return com_vel/total_mass;
}

/**
 * @brief Center a collection of particles around their center of mass.
 *
 * Computes the center of mass with getCenterOfMassPos() and getCenterOfMassVel()
 *
 * @param particles Particles to be centered.
 */
inline void centerParticles(std::vector<BaseParticle>& particles)
{
    const math::Vector3D c_COM_POS = getCenterOfMassPos(particles);
    const math::Vector3D c_COM_VEL = getCenterOfMassVel(particles);

    for (auto& particle: particles) {
        particle.pos -= c_COM_POS;
        particle.vel -= c_COM_VEL;
    }
}

} // namespace enkas::physics
