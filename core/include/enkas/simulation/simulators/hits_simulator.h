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

    void setSystem(const data::System& initial_system) override;
    void step() override;

    [[nodiscard]] double getSystemTime() const override;
    [[nodiscard]] data::System getSystem() const override;

   private:
    void updateParticle(size_t particle_index);
    data::System getPredictedSystem(double time, bool sync_mode) const;
    void calculateAccJrk(const data::System& system,
                         size_t particle_index,
                         math::Vector3D& acc,
                         math::Vector3D& jrk) const;
    void correctParticle(const data::System& pred_system,
                         size_t particle_index,
                         const math::Vector3D& pred_acc,
                         const math::Vector3D& pred_jrk);
    void updateParticleTimeStep(size_t particle_index);

   private:
    HitsSettings settings_;

    double system_time_ = 0.0;    // current time of the system
    const double softening_sqr_;  // squared softening parameters

    data::System system_;                        // current state of the system
    std::vector<math::Vector3D> accelerations_;  // accelerations of the particles
    std::vector<math::Vector3D> jerks_;          // jerks of the particles
    std::vector<math::Vector3D> snaps_;          // snaps of the particles
    std::vector<math::Vector3D> crackles_;       // crackles of the

    std::vector<double> particle_times_;       // particle times
    std::vector<double> particle_time_steps_;  // particle time steps

    std::map<double, size_t> particle_schedule_;  // schedules which particle to update next
};

}  // namespace enkas::simulation
