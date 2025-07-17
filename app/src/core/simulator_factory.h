#pragma once

#include <enkas/simulation/settings/barneshutleapfrog_settings.h>
#include <enkas/simulation/settings/euler_settings.h>
#include <enkas/simulation/settings/hermite_settings.h>
#include <enkas/simulation/settings/hits_settings.h>
#include <enkas/simulation/settings/leapfrog_settings.h>
#include <enkas/simulation/simulation_factory.h>
#include <enkas/simulation/simulation_method.h>
#include <enkas/simulation/simulator.h>

#include <memory>

#include "core/settings.h"

class SimulatorFactory {
public:
    static std::unique_ptr<enkas::simulation::Simulator> create(const Settings& settings);

private:
    static enkas::simulation::EulerSettings getEulerSettings(const Settings& settings);
    static enkas::simulation::LeapfrogSettings getLeapfrogSettings(const Settings& settings);
    static enkas::simulation::HermiteSettings getHermiteSettings(const Settings& settings);
    static enkas::simulation::HitsSettings getHitsSettings(const Settings& settings);
    static enkas::simulation::BarnesHutLeapfrogSettings getBarnesHutLeapfrogSettings(
        const Settings& settings);
};
