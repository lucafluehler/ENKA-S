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

void BarnesHutLeapfrogSimulator::setSystem(const data::System& initial_system) {
    ENKAS_LOG_INFO("Setting up Barnes-Hut Leapfrog simulator with new initial system...");

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
    barneshut_tree_.build(system_);
    barneshut_tree_.updateForces(system_, theta_mac_sqr_, softening_sqr_, accelerations_);

    system_time_ = 0.0;
    ENKAS_LOG_INFO("System setup complete. Simulation ready to start.");
}

void BarnesHutLeapfrogSimulator::step() {
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

    // Calculate accelerations for all particles using Barnes-Hut tree
    barneshut_tree_.build(system_);
    barneshut_tree_.updateForces(system_, theta_mac_sqr_, softening_sqr_, accelerations_);

    // Leapfrog Second "Kick"
    for (size_t i = 0; i < particle_count; ++i) {
        system_.velocities[i] += accelerations_[i] * dt * 0.5;
    }

    // Update system time with time_step
    system_time_ += dt;
}

[[nodiscard]] double BarnesHutLeapfrogSimulator::getSystemTime() const { return system_time_; }

[[nodiscard]] data::System BarnesHutLeapfrogSimulator::getSystem() const { return system_; }

}  // namespace enkas::simulation
