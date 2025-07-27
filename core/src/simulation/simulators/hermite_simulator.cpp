#include <enkas/data/system.h>
#include <enkas/logging/logger.h>
#include <enkas/math/vector3d.h>
#include <enkas/physics/helpers.h>
#include <enkas/simulation/simulators/hermite_simulator.h>

#include <cmath>
#include <vector>

namespace enkas::simulation {

HermiteSimulator::HermiteSimulator(const HermiteSettings& settings)
    : settings_(settings),
      softening_sqr_(settings.softening_parameter * settings.softening_parameter) {}

void HermiteSimulator::initialize(std::shared_ptr<data::System> initial_system,
                                  std::shared_ptr<data::System> system_buffer) {
    ENKAS_LOG_INFO("Setting up Hermite simulator with new initial system...");

    previous_system_ = initial_system;
    system_ = system_buffer;

    const size_t particle_count = previous_system_->count();
    accelerations_.resize(particle_count);
    jerks_.resize(particle_count);
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

void HermiteSimulator::step(std::shared_ptr<data::System> system_buffer,
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

void HermiteSimulator::calculateNextSystemState() {
    if (isStopRequested()) return;

    const size_t particle_count = system_->count();
    if (particle_count == 0) return;

    const auto old_accelerations = accelerations_;
    const auto old_jerks = jerks_;

    const double dt = settings_.time_step;
    const double dt2 = dt * dt;
    const double dt3 = dt2 * dt;

    // Predict particle position and velocity up to order jerk
    for (size_t i = 0; i < particle_count; ++i) {
        system_->positions[i] = previous_system_->positions[i] +
                                previous_system_->velocities[i] * dt +
                                old_accelerations[i] * dt2 * 0.5 + old_jerks[i] * dt3 / 6.0;

        system_->velocities[i] =
            previous_system_->velocities[i] + old_accelerations[i] * dt + old_jerks[i] * dt2 * 0.5;
    }

    // Calculate acceleration and jerk for the entire system
    updateForces();

    // Correct particle position and velocity using hermite scheme
    for (size_t i = 0; i < particle_count; ++i) {
        system_->velocities[i] = previous_system_->velocities[i] +
                                 (old_accelerations[i] + accelerations_[i]) * dt * 0.5 +
                                 (old_jerks[i] - jerks_[i]) * dt2 / 12.0;

        system_->positions[i] =
            previous_system_->positions[i] +
            (previous_system_->velocities[i] + system_->velocities[i]) * dt * 0.5 +
            (old_accelerations[i] - accelerations_[i]) * dt2 / 12.0;
    }

    // Update system time with time_step
    system_time_ += dt;
}

[[nodiscard]] double HermiteSimulator::getSystemTime() const { return system_time_; }

void HermiteSimulator::updateForces() {
    const size_t particle_count = system_->count();
    if (particle_count == 0) return;

    potential_energy_ = 0.0;

    // Reset accelerations and jerks
    std::fill(accelerations_.begin(), accelerations_.end(), math::Vector3D{});
    std::fill(jerks_.begin(), jerks_.end(), math::Vector3D{});

    // Calculate pair-wise accelerations and jerks
    const auto& positions = system_->positions;
    const auto& velocities = system_->velocities;
    const auto& masses = system_->masses;

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
            const math::Vector3D acc_term = r_ij * dist_inv_cubed;

            accelerations_[i] += acc_term * masses[j];
            accelerations_[j] -= acc_term * masses[i];

            // Jerk
            const double r_dot_v = math::dotProduct(r_ij, v_ij);
            const math::Vector3D jerk_term =
                (v_ij - r_ij * (3.0 * r_dot_v * dist_inv * dist_inv)) * dist_inv_cubed;

            jerks_[i] += jerk_term * masses[j];
            jerks_[j] -= jerk_term * masses[i];

            // Potential energy
            potential_energy_ -= masses[i] * masses[j] * dist_inv;
        }
    }
}

}  // namespace enkas::simulation
