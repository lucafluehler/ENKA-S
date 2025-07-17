#include "core/simulator_factory.h"

#include <enkas/logging/logger.h>
#include <enkas/simulation/simulation_factory.h>
#include <enkas/simulation/simulation_settings.h>
#include <enkas/simulation/simulator.h>
#include <enkas/simulation/simulators/barneshutleapfrog_simulator.h>
#include <enkas/simulation/simulators/euler_simulator.h>
#include <enkas/simulation/simulators/hermite_simulator.h>
#include <enkas/simulation/simulators/hits_simulator.h>
#include <enkas/simulation/simulators/leapfrog_simulator.h>

#include <memory>

#include "core/settings/settings.h"

std::unique_ptr<enkas::simulation::Simulator> SimulatorFactory::create(const Settings& settings) {
    auto method = settings.get<SimulationMethod>(SettingKey::SimulationMethod);

    switch (method) {
        case SimulationMethod::Euler:
            return enkas::simulation::Factory::create(getEulerSettings(settings));
        case SimulationMethod::Leapfrog:
            return enkas::simulation::Factory::create(getLeapfrogSettings(settings));
        case SimulationMethod::Hermite:
            return enkas::simulation::Factory::create(getHermiteSettings(settings));
        case SimulationMethod::Hits:
            return enkas::simulation::Factory::create(getHitsSettings(settings));
        case SimulationMethod::BarnesHutLeapfrog:
            return enkas::simulation::Factory::create(getBarnesHutLeapfrogSettings(settings));
        default:
            return nullptr;  // Unsupported generation method
    }
}
