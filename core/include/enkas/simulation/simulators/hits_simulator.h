#pragma once

#include <enkas/data/system.h>
#include <enkas/math/vector3d.h>
#include <enkas/simulation/settings/hits_settings.h>
#include <enkas/simulation/simulator.h>

#include <map>
#include <vector>

namespace enkas::simulation {

class HitsSimulator : public Simulator {
public:
    explicit HitsSimulator(const HitsSettings& settings);

    ~HitsSimulator() override = default;

    void initialize(std::shared_ptr<data::System> initial_system,
                    std::shared_ptr<data::System> system_buffer) override;

    void step(std::shared_ptr<data::System> system_buffer = nullptr,
              std::shared_ptr<data::Diagnostics> diagnostics_buffer = nullptr) override;

    [[nodiscard]] double getSystemTime() const override;

private:
    void updateParticle(size_t particle_index);
    void predictSystem(const data::System& reference_system,
                       data::System& pred_system,
                       double time,
                       bool sync_mode) const;
    void calculateAccJrk(const data::System& system,
                         size_t particle_index,
                         math::Vector3D& acc,
                         math::Vector3D& jrk);
    void correctParticle(const data::System& pred_system,
                         size_t particle_index,
                         const math::Vector3D& pred_acc,
                         const math::Vector3D& pred_jrk);
    void updateParticleTimeStep(size_t particle_index);

private:
    void calculateNextSystemState();

    HitsSettings settings_;

    double system_time_ = 0.0;    // current time of the system
    const double softening_sqr_;  // squared softening parameters

    // state of the system at the previous step
    std::shared_ptr<data::System> system_ = nullptr;       // contains the current system state
    std::shared_ptr<data::System> temp_system_ = nullptr;  // used to predict a system at some time

    std::vector<math::Vector3D> accelerations_;  // accelerations of the particles
    std::vector<math::Vector3D> jerks_;          // jerks of the particles
    std::vector<math::Vector3D> snaps_;          // snaps of the particles
    std::vector<math::Vector3D> crackles_;       // crackles of the particles

    std::vector<double> particle_times_;       // particle times
    std::vector<double> particle_time_steps_;  // particle time steps

    std::map<double, size_t> particle_schedule_;  // schedules which particle to update next
};

}  // namespace enkas::simulation
