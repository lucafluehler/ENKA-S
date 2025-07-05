#pragma once

#include <random>
#include <vector>
#include <cmath>

#include "geometric_algebra.h"
#include "analytics_data.h"
#include "render_data.h"
#include "diagnostics_data.h"

namespace utils {



/**
 * @brief Generate a random 3D vector on the surface of a sphere.
 *
 * This function generates a random 3D vector where the direction is uniformly distributed
 * on the surface of a unit sphere and then scales it by the provided 'norm' value.
 *
 * @param norm The desired magnitude (norm) of the generated vector.
 * @return A Vector3D representing a random vector on the sphere's surface, scaled by 'norm'.
 *
 * @note The function uses a random number generator to create the random vector.
 */
inline math::Vector3D getRandSphere(std::mt19937& gen, double norm = 1.0)
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    math::Vector3D sphere;

    const double c_THETA = std::acos(2.0*dist(gen) - 1.0);
    const double c_PHI = 2.0*M_PI*dist(gen);

    sphere.x = std::sin(c_THETA)*std::cos(c_PHI);
    sphere.y = std::sin(c_THETA)*std::sin(c_PHI);
    sphere.z = std::cos(c_THETA);

    return sphere*norm;
}

/**
 * @brief Concept to check if a type is derived from BaseParticle.
 *
 * @tparam T The type to be checked.
 */
template <typename T>
concept DerivedFromBaseParticle = std::is_base_of_v<BaseParticle, T>;

/**
 * @brief Scale the properties of a vector of particles.
 *
 * This function scales the properties of a vector of particles, including their
 * masses, positions, and velocities to Hénon units. It is intended to be used with
 * particle types derived from BaseParticle.
 *
 * @tparam T The particle type. Must be derived from BaseParticle
 *
 * @param particles The vector of particles to scale.
 * @param abs_e_tot The absolute total energy of the system in units of the provided
 *                  particles.
 * @see Heggie, D. and Methieu, R.; 1986; Standardised units and time scales
 */
template <DerivedFromBaseParticle T>
inline void scaleParticles(std::vector<T>& particles, double abs_e_tot)
{
    double total_mass = 0.0;
    for (const auto& particle: particles) total_mass += particle.mass;

    const double c_MASS_UNIT = total_mass;
    const double c_LENGTH_UNIT = G*std::pow(total_mass, 2)/abs_e_tot/4;
    const double c_TIME_UNIT = G*std::sqrt(std::pow(total_mass, 5)/std::pow(4*abs_e_tot, 3));
    const double c_VELOCITY_UNIT = c_LENGTH_UNIT/c_TIME_UNIT;

    for (auto& particle: particles) {
        particle.mass /= c_MASS_UNIT;
        particle.pos /= c_LENGTH_UNIT;
        particle.vel /= c_VELOCITY_UNIT;
    }

    qDebug() << "----- Unit values -----";
    qDebug() << "Mass Unit:     " << c_MASS_UNIT << " Solar Masses";
    qDebug() << "Length Unit:   " << c_LENGTH_UNIT << " pc";
    qDebug() << "Time Unit:     " << c_TIME_UNIT*3.085677581e13/31556952 << " yr";
    qDebug() << "Velocity Unit: " << c_VELOCITY_UNIT << " km/s";
    qDebug() << "-----------------------";
    qDebug();
}

template <DerivedFromBaseParticle T>
inline RenderData getRenderData(double global_time, const std::vector<T> particles)
{
    RenderData data;

    data.positions.reserve(particles.size());
    for (const auto& particle : particles) {
        data.positions.push_back(particle.pos);
    }

    data.time = global_time;

    data.com_position = getCenterOfMassPos(particles);

    return data;
}

template <DerivedFromBaseParticle T>
inline DiagnosticsData getDiagnosticsData( double global_time, double e_pot
                                         , const std::vector<T> particles )
{
    DiagnosticsData data;

    data.time = global_time;

    data.e_kin = getKineticEnergy(particles);
    data.e_pot = e_pot;
    data.L_tot = getAngularMomentum(particles).norm();

    data.com_pos = getCenterOfMassPos(particles);
    data.com_vel = getCenterOfMassVel(particles);

    data.ms_vel = 2.0*data.e_kin;
    data.r_vir = 1.0/std::abs(data.e_pot)/2;
    data.t_cr = 2.0*data.r_vir/std::sqrt(data.ms_vel);

    return data;
}

template <DerivedFromBaseParticle T>
inline AnalyticsData getAnalyticsData(double global_time, const std::vector<T> particles)
{
    AnalyticsData data;

    data.time = global_time;

    // Lagrange radii
    std::vector<T> sorted_system = particles;
    std::sort(sorted_system.begin(), sorted_system.end(), [](const auto& a, const auto& b) {
        return a.pos.norm2() < b.pos.norm2();
    });

    double total_mass = 0.0;
    for (const auto& particle : sorted_system) total_mass += particle.mass;

    double cumulative_mass = 0.0;
    for (const auto& particle : sorted_system) {
        cumulative_mass += particle.mass;

        if (data.mass_radius_10 == 0.0 && cumulative_mass >= 0.10*total_mass) {
            data.mass_radius_10 = particle.pos.norm();
        }

        if (data.mass_radius_25 == 0.0 && cumulative_mass >= 0.25*total_mass) {
            data.mass_radius_25 = particle.pos.norm();
        }

        if (data.half_mass_radius == 0.0 && cumulative_mass >= 0.50*total_mass) {
            data.half_mass_radius = particle.pos.norm();
        }

        if (data.mass_radius_75 == 0.0 && cumulative_mass >= 0.75*total_mass) {
            data.mass_radius_75 = particle.pos.norm();
        }

        if (data.mass_radius_90 == 0.0 && cumulative_mass >= 0.90*total_mass) {
            data.mass_radius_90 = particle.pos.norm();
        }
    }

    // Time scales
    const double c_N = particles.size();
    const double c_HALF_MASS_FACTOR = std::pow(data.half_mass_radius, 3.0/2.0);
    data.t_rh = 0.138*c_HALF_MASS_FACTOR*c_N/std::log(0.11*c_N);
    data.t_dh = 1.58*c_HALF_MASS_FACTOR;

    return data;
}

} // namespace utils
