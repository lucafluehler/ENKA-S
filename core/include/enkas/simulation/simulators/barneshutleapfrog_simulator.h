#pragma once

#include <vector>
#include <memory>

#include <enkas/data/system.h>
#include <enkas/simulation/simulator.h>
#include <enkas/simulation/simulators/barneshut_tree.h>
#include <enkas/simulation/settings/barneshutleapfrog_settings.h>

namespace enkas::simulation {

class BarnesHutLeapfrogSimulator : public Simulator
{
public:
    explicit BarnesHutLeapfrogSimulator(const BarnesHutLeapfrogSettings& settings);

    ~BarnesHutLeapfrogSimulator() override = default;

    void setSystem(const data::System& initial_system) override;
    void step() override;

    [[nodiscard]] double getSystemTime() const override;
    [[nodiscard]] data::System getSystem() const override;

private:
    BarnesHutLeapfrogSettings settings_; 

    double system_time_ = 0.0; // current time of the system
    const double theta_mac_sqr_; // squared multipole acceptance criterion
    const double softening_sqr_; // squared softening parameters

    data::System system_; // current state of the system
    BarnesHutTree barneshut_tree_; // Barnes-Hut tree for acceleration calculations
    std::vector<math::Vector3D> accelerations_; // accelerations of particles
};

} // namespace enkas::simulation
