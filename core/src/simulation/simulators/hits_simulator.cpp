#include <enkas/data/system.h>
#include <enkas/logging/logger.h>
#include <enkas/math/vector3d.h>
#include <enkas/physics/helpers.h>
#include <enkas/simulation/simulators/hits_simulator.h>

#include <cmath>
#include <vector>

namespace enkas::simulation {

HitsSimulator::HitsSimulator(const HitsSettings& settings)
    : settings_(settings),
      softening_sqr_(settings.softening_parameter * settings.softening_parameter) {}

void HitsSimulator::initialize(std::shared_ptr<data::System> initial_system,
                               std::shared_ptr<data::System> system_buffer) {
    ENKAS_LOG_INFO("Setting up Euler simulator with new initial system...");

    system_ = initial_system;
    temp_system_ = system_buffer;

    // Resize vectors to the number of particles in the system
    const size_t particle_count = system_->count();
    accelerations_.resize(particle_count);
    jerks_.resize(particle_count);
    snaps_.resize(particle_count);
    crackles_.resize(particle_count);
    particle_times_.resize(particle_count, 0.0);
    particle_time_steps_.resize(particle_count, 0.0);
    ENKAS_LOG_DEBUG("System contains {} particles.", particle_count);

    // Scale particles to Hénon Units.
    const double e_kin = physics::getKineticEnergy(*system_);
    const double e_pot = physics::getPotentialEnergy(*system_, softening_sqr_);
    const double total_energy = std::abs(e_kin + e_pot * physics::G);
    physics::scaleToHenonUnits(*system_, total_energy);
    ENKAS_LOG_DEBUG("Scaling to Hénon units with total energy: {:.4e}", total_energy);

    // Update system masses after scaling
    temp_system_->masses = system_->masses;

    // Initialize time step of each particle using Aarseth's initialization formula
    // (MULTIPLE TIME SCALES, 1985)
    ENKAS_LOG_INFO("Initializing accelerations, jerks and time steps...");
    for (size_t i = 0; i < particle_count; i++) {
        auto& acc = accelerations_[i];
        auto& jrk = jerks_[i];
        calculateAccJrk(*system_, i, acc, jrk);

        const double eta = settings_.time_step_parameter;
        if (acc.norm() == 0.0 || jrk.norm() == 0.0) {
            particle_time_steps_[i] = eta;
        } else {
            particle_time_steps_[i] = eta * acc.norm() / (jrk.norm());
        }
        particle_schedule_[particle_time_steps_[i]] = i;
    }

    system_time_ = 0.0;
    ENKAS_LOG_INFO("System setup complete. Simulation ready to start.");
}

void HitsSimulator::step(std::shared_ptr<data::System> system_buffer,
                         std::shared_ptr<data::Diagnostics> diagnostics_buffer) {
    calculateNextSystemState();

    // Synchronize the system if a buffer is provided
    if (system_buffer) {
        predictSystem(*system_, *system_buffer, system_time_, true);
        system_buffer->masses = system_->masses;
    }

    // If diagnostics buffer is provided, fill it with the current diagnostics data
    if (diagnostics_buffer) {
        predictSystem(*system_, *temp_system_, system_time_, true);
        physics::fillDiagnostics(*temp_system_,
                                 physics::getPotentialEnergy(*temp_system_, softening_sqr_),
                                 *diagnostics_buffer);
    }
}

void HitsSimulator::calculateNextSystemState() {
    if (particle_schedule_.empty()) return;

    // Choose first particle in map as min_i(t_i + dt_i)
    auto node = particle_schedule_.extract(particle_schedule_.begin());
    auto& particle_time = node.key();
    auto& particle_index = node.mapped();

    system_time_ = particle_time;
    updateParticle(particle_index);

    // Update particle time to system time and reschedule it
    particle_times_[particle_index] = system_time_;
    particle_time += particle_time_steps_[particle_index];
    particle_schedule_.insert(std::move(node));
}

[[nodiscard]] double HitsSimulator::getSystemTime() const { return system_time_; }

void HitsSimulator::updateParticle(size_t particle_index) {
    // Predictor
    predictSystem(*system_, *temp_system_, system_time_, false);

    // Evaluator
    math::Vector3D acc;
    math::Vector3D jrk;
    calculateAccJrk(*temp_system_, particle_index, acc, jrk);

    // Corrector
    correctParticle(*temp_system_, particle_index, acc, jrk);

    // Update time step
    updateParticleTimeStep(particle_index);
}

void HitsSimulator::predictSystem(const data::System& reference_system,
                                  data::System& pred_system,
                                  double time,
                                  bool sync_mode) const {
    const size_t particle_count = reference_system.count();

    for (size_t i = 0; i < particle_count; i++) {
        const double dt = time - particle_times_[i];
        const double dt2 = dt * dt;
        const double dt3 = dt2 * dt;

        pred_system.positions[i] = reference_system.positions[i] +
                                   reference_system.velocities[i] * dt +
                                   accelerations_[i] * dt2 / 2.0 + jerks_[i] * dt3 / 6.0;

        pred_system.velocities[i] =
            reference_system.velocities[i] + accelerations_[i] * dt + jerks_[i] * dt2 / 2.0;

        // If the particles need to be synced for data retrieval, we need
        // to use higher order terms for our taylor series.
        if (!sync_mode) continue;

        const double dt4 = dt3 * dt;
        const double dt5 = dt4 * dt;

        pred_system.positions[i] += snaps_[i] * dt4 / 24.0 + crackles_[i] * dt5 / 120.0;

        pred_system.velocities[i] += snaps_[i] * dt3 / 6.0 + crackles_[i] * dt4 / 24.0;
    }

    // Update the system's masses
    pred_system.masses = reference_system.masses;
}

void HitsSimulator::calculateAccJrk(const data::System& system,
                                    size_t i,  // particle index
                                    math::Vector3D& acc,
                                    math::Vector3D& jrk) {
    acc.fill(0.0);
    jrk.fill(0.0);

    const auto& positions = system.positions;
    const auto& velocities = system.velocities;
    const auto& masses = system.masses;

    for (size_t j = 0; j < system.count(); j++) {
        if (j == i) continue;

        // Acceleration
        const math::Vector3D r_ij = positions[j] - positions[i];
        const double dist_sq = r_ij.norm2() + softening_sqr_;
        const double dist_inv = 1.0 / std::sqrt(dist_sq);
        const double dist_inv_cubed = dist_inv * dist_inv * dist_inv;

        acc += r_ij * masses[j] * dist_inv_cubed;

        // Jerk
        const math::Vector3D v_ij = velocities[j] - velocities[i];
        const double r_dot_v = math::dotProduct(r_ij, v_ij);
        const math::Vector3D jerk_term = v_ij - r_ij * (3.0 * r_dot_v * dist_inv * dist_inv);

        jrk += jerk_term * masses[j] * dist_inv_cubed;
    }
}

void HitsSimulator::correctParticle(const data::System& pred_system,
                                    size_t i,  // particle index
                                    const math::Vector3D& pred_acc,
                                    const math::Vector3D& pred_jrk) {
    const double dt = system_time_ - particle_times_[i];
    const double dt2 = dt * dt;
    const double dt3 = dt2 * dt;

    // Hermite interpolation of snap multiplied by dt3 as to avoid higher
    // powers of dt
    const math::Vector3D snp_dt3 =
        (accelerations_[i] - pred_acc) * dt * (-6.0) - (jerks_[i] * 4.0 + pred_jrk * 2.0) * dt2;

    // Hermite interpolation of crackle multiplied by dt3 as to avoid higher
    // powers of dt
    const math::Vector3D crk_dt3 =
        (accelerations_[i] - pred_acc) * 12.0 + (jerks_[i] + pred_jrk) * 6.0 * dt;

    system_->positions[i] = pred_system.positions[i] + snp_dt3 * dt / 24.0 + crk_dt3 * dt2 / 120.0;
    system_->velocities[i] = pred_system.velocities[i] + snp_dt3 / 6.0 + crk_dt3 * dt / 24.0;

    //    system_.velocities[i] +=   (accelerations_[i] + pred_acc)*dt/2
    //                             + (jerks_[i] - pred_jrk)*dt2/12;

    //    system_.positions[i]  +=   (particle.vel + pred_system.velocities[i])*dt/2
    //                             + (accelerations_[i] - pred_acc)*dt2/12;

    accelerations_[i] = pred_acc;
    jerks_[i] = pred_jrk;
    snaps_[i] = snp_dt3 / dt3 + crk_dt3 / dt2;
    crackles_[i] = crk_dt3 / dt3;
}

void HitsSimulator::updateParticleTimeStep(size_t particle_index) {
    auto& particle_dt = particle_time_steps_[particle_index];

    const auto& acc = accelerations_[particle_index];
    const auto& jrk = jerks_[particle_index];
    const auto& snp = snaps_[particle_index];
    const auto& crk = crackles_[particle_index];

    const double a = acc.norm() * snp.norm() + jrk.norm2();
    const double b = jrk.norm() * crk.norm() + snp.norm2();

    double new_dt = settings_.time_step_parameter;
    if (a != 0.0 && b != 0.0) {
        new_dt = std::sqrt(a * settings_.time_step_parameter / b);
    }

    // Set an upper limit to the relative change of dt
    const double mu = 0.3;  // allow for 30 % change
    const double upper_limit = particle_dt * (1 + mu);
    particle_dt = std::min(upper_limit, new_dt);

    //    // Set an upper and lower limit to the absolute dt
    //    const double eta = 20.0;
    //    const double min_dt = settings_.time_step_parameter/eta;
    //    const double max_dt = settings_.time_step_parameter*eta;
    //    particle_dt = std::max(min_dt, std::min(max_dt, particle_dt));
}

}  // namespace enkas::simulation
