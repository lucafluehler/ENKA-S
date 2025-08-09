#include <enkas/data/system.h>
#include <enkas/logging/logger.h>
#include <enkas/math/vector3d.h>
#include <enkas/physics/helpers.h>
#include <enkas/simulation/simulators/barneshut_tree.h>
#include <enkas/simulation/simulators/barneshutleapfrog_simulator.h>

namespace enkas::simulation {

BarnesHutLeapfrogSimulator::BarnesHutLeapfrogSimulator(const BarnesHutLeapfrogSettings& settings)
    : settings_(settings),
      theta_mac_sqr_(settings.theta_mac * settings.theta_mac),
      softening_sqr_(settings.softening_parameter * settings.softening_parameter) {}

void BarnesHutLeapfrogSimulator::initialize(std::shared_ptr<data::System> initial_system,
                                            std::shared_ptr<data::System> system_buffer) {
    ENKAS_LOG_INFO("Setting up Leapfrog simulator with new initial system...");

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
    updateForces(*previous_system_);

    system_time_ = 0.0;
    ENKAS_LOG_INFO("System setup complete. Simulation ready to start.");
}

void BarnesHutLeapfrogSimulator::step(std::shared_ptr<data::System> system_buffer,
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

void BarnesHutLeapfrogSimulator::calculateNextSystemState() {
    if (isStopRequested()) return;

    const size_t particle_count = system_->count();
    if (particle_count == 0) return;

    const double dt = settings_.time_step;

    // Leapfrog First "Kick"
    for (size_t i = 0; i < particle_count; ++i) {
        system_->velocities[i] = previous_system_->velocities[i] + accelerations_[i] * dt * 0.5;
    }

    // Leapfrog "Drift"
    for (size_t i = 0; i < particle_count; ++i) {
        system_->positions[i] = previous_system_->positions[i] + system_->velocities[i] * dt;
    }

    // Calculate accelerations for all particles using Barnes-Hut tree
    updateForces(*system_);

    // Leapfrog Second "Kick"
    for (size_t i = 0; i < particle_count; ++i) {
        system_->velocities[i] += accelerations_[i] * dt * 0.5;
    }

    // Update system time with time_step
    system_time_ += dt;
}

[[nodiscard]] double BarnesHutLeapfrogSimulator::getSystemTime() const { return system_time_; }

void BarnesHutLeapfrogSimulator::updateForces(const data::System& system) {
    barneshut_tree_.build(system);
    potential_energy_ =
        barneshut_tree_.updateForces(system, theta_mac_sqr_, softening_sqr_, accelerations_);
}

}  // namespace enkas::simulation
