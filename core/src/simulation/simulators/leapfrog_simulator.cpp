#include <vector>
#include <cmath>

#include <enkas/simulation/simulators/leapfrog_simulator.h>
#include <enkas/data/system.h>
#include <enkas/physics/helpers.h>
#include <enkas/math/vector3d.h>

namespace enkas::simulation {

LeapfrogSimulator::LeapfrogSimulator(const LeapfrogSettings& settings)
    : settings_(settings)
    , softening_sqr_(settings.softening_parameter*settings.softening_parameter)
{}

void LeapfrogSimulator::setSystem(data::System initial_system)
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

void LeapfrogSimulator::step()
{
    if (isStopRequested()) return;

    const size_t particle_count = system_.count();
    if (particle_count == 0) return;

    const double dt = settings_.time_step;

    // Leapfrog First "Kick"
    for (size_t i = 0; i < particle_count; ++i) {
        system_.velocities[i] += accelerations_[i]*dt*0.5;
    }

    // Leapfrog "Drift"
    for (size_t i = 0; i < particle_count; ++i) {
        system_.positions[i] += system_.velocities[i]*dt;
    }

    // Calculate acceleration and potential energy for the entire system
    updateForcesAndEnergy();

    // Leapfrog Second "Kick"
    for (size_t i = 0; i < particle_count; ++i) {
        system_.velocities[i] += accelerations_[i]*dt*0.5;
    }

    // Update global time with time_step
    system_time_ += dt;
}

[[nodiscard]] double LeapfrogSimulator::getSystemTime() const { return system_time_; }

[[nodiscard]] data::System LeapfrogSimulator::getSystem() const { return system_; }

void LeapfrogSimulator::updateForcesAndEnergy()
{
    const size_t particle_count = system_.count();
    if (particle_count == 0) return;

    // Reset accelerations and potential energy
    potential_energy_ = 0.0;
    std::fill(m_accelerations.begin(), m_accelerations.end(), math::Vector3D{});

    // Calculate pair-wise accelerations and potential energy simultaneously
    const auto& positions = m_system_data.positions;
    const auto& masses = m_system_data.masses;

    for (size_t i = 0; i < particle_count; i++) {
        if (isStopRequested()) return;
        for (size_t j = i + 1; j < particle_count; j++) {
            const math::Vector3D r_ij = positions[j] - positions[i];
            const double dist_sqr = r_ij.norm2() + softening_sqr_;

            if (dist_sqr <= 0) continue;

            const double dist_inv = 1.0/std::sqrt(dist_sqr);
            const double dist_inv_cubed = dist_inv*dist_inv*dist_inv;

            particle_i.acc += r_ij*masses[j]*dist_inv_cubed;
            particle_j.acc -= r_ij*masses[i]*dist_inv_cubed;

            potential_energy_ -= masses[i]*masses[j].mass*dist_inv;
        }
    }
}

} // namespace enkas::simulation
