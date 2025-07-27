#pragma once

#include <enkas/data/system.h>
#include <enkas/math/vector3d.h>
#include <enkas/simulation/settings/euler_settings.h>
#include <enkas/simulation/simulator.h>

#include <vector>

namespace enkas::simulation {

class EulerSimulator : public Simulator {
public:
    explicit EulerSimulator(const EulerSettings& settings);

    ~EulerSimulator() override = default;

    void initialize(std::shared_ptr<data::System> initial_system,
                    std::shared_ptr<data::System> system_buffer) override;

    void step(std::shared_ptr<data::System> system_buffer = nullptr,
              std::shared_ptr<data::Diagnostics> diagnostics_buffer = nullptr) override;

    [[nodiscard]] double getSystemTime() const override;

private:
    void updateForces();
    void calculateNextSystemState();

private:
    EulerSettings settings_;

    double system_time_ = 0.0;       // current time of the system
    const double softening_sqr_;     // squared softening parameters
    double potential_energy_ = 0.0;  // potential energy of the system

    // state of the system at the previous step
    std::shared_ptr<data::System> previous_system_ = nullptr;
    std::shared_ptr<data::System> system_ = nullptr;  // system to write the new state to

    std::vector<math::Vector3D> accelerations_;  // accelerations of the particles
};

}  // namespace enkas::simulation
