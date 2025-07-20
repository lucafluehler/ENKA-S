#pragma once

#include <enkas/simulation/settings/barneshutleapfrog_settings.h>
#include <enkas/simulation/settings/euler_settings.h>
#include <enkas/simulation/settings/hermite_settings.h>
#include <enkas/simulation/settings/hits_settings.h>
#include <enkas/simulation/settings/leapfrog_settings.h>
#include <enkas/simulation/simulation_factory.h>
#include <enkas/simulation/simulator.h>

#include <memory>

#include "core/settings/settings.h"

/**
 * @brief Factory class for creating different types of simulators based on settings.
 */
class SimulatorFactory {
    using Simulator = enkas::simulation::Simulator;
    using EulerSettings = enkas::simulation::EulerSettings;
    using LeapfrogSettings = enkas::simulation::LeapfrogSettings;
    using HermiteSettings = enkas::simulation::HermiteSettings;
    using HitsSettings = enkas::simulation::HitsSettings;
    using BarnesHutLeapfrogSettings = enkas::simulation::BarnesHutLeapfrogSettings;

public:
    /**
     * @brief Creates a simulator based on the provided settings.
     * Relies on SettingKey::SimulationMethod to determine the type of simulator.
     * @param settings The settings to configure the simulator.
     * @return A unique pointer to the created simulator. If the settings do not match any known
     * simulator type, returns nullptr.
     */
    static std::unique_ptr<Simulator> create(const Settings& settings);

private:
    static EulerSettings getEulerSettings(const Settings& settings);
    static LeapfrogSettings getLeapfrogSettings(const Settings& settings);
    static HermiteSettings getHermiteSettings(const Settings& settings);
    static HitsSettings getHitsSettings(const Settings& settings);
    static BarnesHutLeapfrogSettings getBarnesHutLeapfrogSettings(const Settings& settings);
};
