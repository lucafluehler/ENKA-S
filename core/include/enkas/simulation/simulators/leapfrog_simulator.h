#pragma once

#include <vector>

#include <enkas/data/system.h>
#include <enkas/math/vector3d.h>
#include <enkas/simulation/simulator.h>
#include <enkas/simulation/settings/leapfrog_settings.h>

namespace enkas::simulation {

class LeapfrogSimulator : public Simulator {
public:
    explicit LeapfrogSimulator(const LeapfrogSettings& settings);

    ~LeapfrogSimulator() override = default;

    void setSystem(data::System initial_system) override;
    void step() override;

    [[nodiscard]] double getSystemTime() const override;
    [[nodiscard]] data::System getSystem() const override;

private:
    /**
     * @brief Calculates the accelerations and potential energy of the system.
     */
    void updateForcesAndEnergy();

private:
    LeapfrogSettings settings_;

    double system_time_ = 0.0; // current time of the system
    double potential_energy_ = 0.0; // total potential energy of the system
    const double softening_sqr_; // squared softening parameters

    data::System system_; // current state of the system
    std::vector<math::Vector3D> accelerations_; // accelerations of the particles
};

} // namespace enkas::simulation