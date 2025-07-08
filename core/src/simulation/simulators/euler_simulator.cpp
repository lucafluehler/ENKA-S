#include <vector>
#include <cmath>

#include <enkas/simulation/simulators/euler_simulator.h>
#include <enkas/data/system.h>
#include <enkas/physics/helpers.h>
#include <enkas/math/vector3d.h>

namespace enkas::simulation {

EulerSimulator::EulerSimulator(const EulerSettings& settings)
    : settings_(settings)
    , softening_sqr_(settings.softening_parameter*settings.softening_parameter)
{}

void EulerSimulator::setSystem(const data::System& initial_system)
{
    system_ = initial_system;

    const size_t particle_count = system_.count();
    accelerations_.resize(particle_count);

    // Scale particles to Hénon Units
    const double e_kin = physics::getKineticEnergy(system_);
    const double e_pot = physics::getPotentialEnergy(system_, softening_sqr_);
    physics::scaleToHenonUnits(system_, std::abs(e_kin + e_pot*physics::G));
    
    // Initialize accelerations vector
    updateForces();

    system_time_ = 0.0;
}

void EulerSimulator::evolveSystem()
{
    if (isStopRequested()) return;

    // Calculate acceleration and potential energy for the entire system
    updateForces();

    const double dt = settings_.time_step;

    // Calculate the new position and velocity of each particle using the
    // previously calculated acceleration
    for (size_t i = 0; i < particle_count; ++i) {
        system_.positions[i] += system_.velocities[i]*dt;
        system_.velocities[i] += accelerations_[i]*dt;
    }

    // Update global time with time_step
    system_time_ += dt;
}

[[nodiscard]] double EulerSimulator::getSystemTime() const { return system_time_; }

[[nodiscard]] data::System EulerSimulator::getSystem() const { return system_; }

void EulerSimulator::updateForces()
{
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

            const double dist_inv = 1.0/std::sqrt(dist_sqr);
            const double dist_inv_cubed = dist_inv*dist_inv*dist_inv;

            particle_i.acc += r_ij*masses[j]*dist_inv_cubed;
            particle_j.acc -= r_ij*masses[i]*dist_inv_cubed;
        }
    }
}

} // namespace enkas::simulation
