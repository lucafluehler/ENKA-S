#include <vector>
#include <cmath>

#include <enkas/simulation/simulators/hermite_simulator.h>
#include <enkas/data/system.h>
#include <enkas/physics/helpers.h>
#include <enkas/math/vector3d.h>

namespace enkas::simulation {

HermiteSimulator::HermiteSimulator(const HermiteSettings& settings)
    : settings_(settings)
    , softening_sqr_(settings.softening_parameter*settings.softening_parameter)
{}

void HermiteSimulator::setSystem(data::System initial_system)
{
    system_ = std::move(initial_system);

    // Calculate total energy
    updateForcesAndEnergy();
    const double total_energy = std::abs(physics::getKineticEnergy(system_) + potential_energy_*physics::G);

    // Scale particles to Hénon Units
    physics::scaleToHenonUnits(system_, total_energy);
    updateForcesAndEnergy();

    system_time_ = 0.0;
}

void HermiteSimulator::step()
{
    if (isStopRequested()) return;

    const size_t particle_count = system_.count();
    if (particle_count == 0) return;

    const auto old_positions = system_.positions;
    const auto old_velocities = system_.velocities;
    const auto old_accelerations = accelerations_;
    const auto old_jerks = jerks_;

    const double dt  = m_settings.time_step;
    const double dt2 = dt*dt;
    const double dt3 = dt2*dt;

    // Predict particle position and velocity up to order jerk
    for (size_t i = 0; i < particle_count; ++i) {
        system_.positions[i] += old_velocities[i]*dt +
                                old_accelerations[i]*dt2*0.5 +
                                old_jerks[i]*dt3/6.0;
        
        system_.velocities[i] += old_accelerations[i]*dt +
                                 old_jerks[i]*dt2*0.5;
    }

    // Calculate acceleration, jerk and potential energy for the entire system
    updateForcesAndEnergy();

    // Correct particle position and velocity using hermite scheme
    for (size_t i = 0; i < particle_count; ++i) {
        system_.velocities[i] = old_velocities[i] +
                                (old_accelerations[i] + accelerations_[i])*dt*0.5 +
                                (old_jerks[i] - jerks_[i])*dt2/12.0;

        system_.positions[i] = old_positions[i] +
                               (old_velocities[i] + system_.velocities[i])*dt*0.5 +
                               (old_accelerations[i] - accelerations_[i])*dt2/12.0;
    }

    // Update system time with time_step
    system_time_ += dt;
}

[[nodiscard]] double HermiteSimulator::getSystemTime() const { return system_time_; }

[[nodiscard]] data::System HermiteSimulator::getSystem() const { return system_; }

//------------------------------------------------------------------------------------------

void HermiteSimulator::updateForcesAndEnergy()
{
    const size_t particle_count = system_.count();
    if (particle_count == 0) return;

    // Reset accelerations, jerks and potential energy
    potential_energy_ = 0.0;
    std::fill(accelerations_.begin(), accelerations_.end(), math::Vector3D{});
    std::fill(jerks_.begin(), jerks_.end(), math::Vector3D{});

    // Calculate pair-wise accelerations and epot simultaneously
    const auto& positions = system_.positions;
    const auto& velocities = system_.velocities;
    const auto& masses = system_.masses;

    for (size_t i = 0; i < particle_count; ++i) {
        if (isStopRequested()) return;
        for (size_t j = i + 1; j < particle_count; j++) {
            const math::Vector3D r_ij = positions[j] - positions[i];
            const math::Vector3D v_ij = velocities[j] - velocities[i];
            const double dist_sq = r_ij.norm2() + softening_sqr_;

            if (dist_sq <= 0) continue;

            const double dist_inv = 1.0 / std::sqrt(dist_sq);
            const double dist_inv_cubed = dist_inv * dist_inv * dist_inv;

            // Acceleration
            const math::Vector3D acc_term  = r_ij*dist_inv_cubed;

            accelerations_[i] += acc_term*masses[j];
            accelerations_[j] -= acc_term*masses[i];

            // Jerk
            const double r_dot_v = math::dotProduct(r_ij, v_ij);
            const math::Vector3D jerk_term = (v_ij - r_ij*(3.0*r_dot_v*dist_inv*dist_inv))*dist_inv_cubed;

            jerks_[i] += jerk_term*masses[j];
            jerks_[j] -= jerk_term*masses[i];

            // Potential Energy
            potential_energy_ -= masses[i]*masses[j]*dist_inv;
        }
    }
}

} // namespace enkas::simulation
