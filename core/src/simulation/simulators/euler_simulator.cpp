#include <enkas/data/system.h>
#include <enkas/logging/logger.h>
#include <enkas/math/vector3d.h>
#include <enkas/physics/helpers.h>
#include <enkas/simulation/simulators/euler_simulator.h>

#include <cmath>
#include <vector>

namespace enkas::simulation {

EulerSimulator::EulerSimulator(const EulerSettings& settings)
    : settings_(settings),
      softening_sqr_(settings.softening_parameter * settings.softening_parameter) {}

void EulerSimulator::initialize(std::shared_ptr<data::System> initial_system,
                                std::shared_ptr<data::System> system_buffer) {
    ENKAS_LOG_INFO("Setting up Euler simulator with new initial system...");

    previous_system_ = initial_system;
    system_ = system_buffer;

    const size_t particle_count = previous_system_->count();
    accelerations_.resize(particle_count);
    ENKAS_LOG_DEBUG("System contains {} particles.", particle_count);

    // Scale particles to Hénon Units
    const double e_kin = physics::getKineticEnergy(*previous_system_);
    const double e_pot = physics::getPotentialEnergy(*previous_system_, softening_sqr_);
    const double total_energy = std::abs(e_kin + e_pot * physics::G);
    physics::scaleToHenonUnits(*previous_system_, total_energy);
    ENKAS_LOG_DEBUG("Scaling to Hénon units with total energy: {:.4e}.", total_energy);

    // Update system masses after scaling
    system_->masses = previous_system_->masses;

    // Initialize accelerations vector
    ENKAS_LOG_INFO("Initializing accelerations...");
    updateForces();

    system_time_ = 0.0;
    ENKAS_LOG_INFO("System setup complete. Simulation ready to start.");
}

void EulerSimulator::step(std::shared_ptr<data::System> system_buffer,
                          std::shared_ptr<data::Diagnostics> diagnostics_buffer) {
    // Use new memory buffer if provided, otherwise use the existing one
    if (system_buffer) {
        system_ = system_buffer;
        system_->masses = previous_system_->masses;
    }

    calculateNextSystemState();

    // If diagnostics buffer is provided, fill it with the current diagnostics data
    if (diagnostics_buffer) {
        physics::fillDiagnostics(*system_, potential_energy_, *diagnostics_buffer);
    }

    // Swap the previous system with the new one
    std::swap(previous_system_, system_);
}

void EulerSimulator::calculateNextSystemState() {
    if (isStopRequested()) return;

    const size_t particle_count = system_->count();
    if (particle_count == 0) return;

    // Calculate accelerations and potential energy
    updateForces();

    // Update positions and velocities
    const double dt = settings_.time_step;
    for (size_t i = 0; i < system_->count(); ++i) {
        system_->positions[i] =
            previous_system_->positions[i] + previous_system_->velocities[i] * dt;
        system_->velocities[i] = previous_system_->velocities[i] + accelerations_[i] * dt;
    }

    // Update system time
    system_time_ += dt;
}

[[nodiscard]] double EulerSimulator::getSystemTime() const { return system_time_; }

void EulerSimulator::updateForces() {
    const size_t particle_count = previous_system_->count();
    if (particle_count == 0) return;

    potential_energy_ = 0.0;

    // Reset accelerations to zero
    std::fill(accelerations_.begin(), accelerations_.end(), math::Vector3D{});

    // Calculate pair-wise accelerations
    const auto& positions = previous_system_->positions;
    const auto& masses = previous_system_->masses;

    for (size_t i = 0; i < particle_count; i++) {
        if (isStopRequested()) return;
        for (size_t j = i + 1; j < particle_count; j++) {
            const math::Vector3D r_ij = positions[j] - positions[i];
            const double dist_sqr = r_ij.norm2() + softening_sqr_;

            if (dist_sqr <= 0) continue;

            const double dist_inv = 1.0 / std::sqrt(dist_sqr);
            const double dist_inv_cubed = dist_inv * dist_inv * dist_inv;

            accelerations_[i] += r_ij * masses[j] * dist_inv_cubed;
            accelerations_[j] -= r_ij * masses[i] * dist_inv_cubed;

            potential_energy_ -= masses[i] * masses[j] * dist_inv;
        }
    }
}

}  // namespace enkas::simulation
