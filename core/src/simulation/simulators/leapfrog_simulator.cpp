#include <enkas/data/system.h>
#include <enkas/logging/logger.h>
#include <enkas/math/vector3d.h>
#include <enkas/physics/helpers.h>
#include <enkas/simulation/simulators/leapfrog_simulator.h>

#include <cmath>
#include <vector>

namespace enkas::simulation {

LeapfrogSimulator::LeapfrogSimulator(const LeapfrogSettings& settings)
    : settings_(settings),
      softening_sqr_(settings.softening_parameter * settings.softening_parameter) {}

void LeapfrogSimulator::setSystem(const data::System& initial_system) {
    ENKAS_LOG_INFO("Setting up Leapfrog simulator with new initial system...");

    system_ = initial_system;

    const size_t particle_count = system_.count();
    accelerations_.resize(particle_count);
    ENKAS_LOG_DEBUG("  System contains {} particles.", particle_count);

    // Scale particles to Hénon Units
    const double e_kin = physics::getKineticEnergy(system_);
    const double e_pot = physics::getPotentialEnergy(system_, softening_sqr_);
    const double total_energy = std::abs(e_kin + e_pot * physics::G);
    physics::scaleToHenonUnits(system_, total_energy);
    ENKAS_LOG_DEBUG("  Scaling to Hénon units with total energy: {}", total_energy);

    // Initialize accelerations vector
    ENKAS_LOG_INFO("  Initializing accelerations...");
    updateForces();

    system_time_ = 0.0;
    ENKAS_LOG_INFO("System setup complete. Simulation ready to start.");
}

void LeapfrogSimulator::step() {
    if (isStopRequested()) return;

    const size_t particle_count = system_.count();
    if (particle_count == 0) return;

    const double dt = settings_.time_step;

    // Leapfrog First "Kick"
    for (size_t i = 0; i < particle_count; ++i) {
        system_.velocities[i] += accelerations_[i] * dt * 0.5;
    }

    // Leapfrog "Drift"
    for (size_t i = 0; i < particle_count; ++i) {
        system_.positions[i] += system_.velocities[i] * dt;
    }

    // Calculate accelerations for all particles
    updateForces();

    // Leapfrog Second "Kick"
    for (size_t i = 0; i < particle_count; ++i) {
        system_.velocities[i] += accelerations_[i] * dt * 0.5;
    }

    // Update system time with time_step
    system_time_ += dt;
}

[[nodiscard]] double LeapfrogSimulator::getSystemTime() const { return system_time_; }

[[nodiscard]] data::System LeapfrogSimulator::getSystem() const { return system_; }

void LeapfrogSimulator::updateForces() {
    const size_t particle_count = system_.count();
    if (particle_count == 0) return;

    // Reset accelerations to zero
    std::fill(accelerations_.begin(), accelerations_.end(), math::Vector3D{});

    // Calculate pair-wise accelerations
    const auto& positions = system_.positions;
    const auto& masses = system_.masses;

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
        }
    }
}

}  // namespace enkas::simulation
