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

std::unique_ptr<enkas::simulation::Simulator> SimulatorFactory::create(const Settings& settings) {
    auto method_str = settings.get<std::string>("SimulationMethod");
    auto method = enkas::simulation::stringToMethod(method_str).value();

    switch (method) {
        case enkas::simulation::Method::Euler:
            return enkas::simulation::Factory::create(getEulerSettings(settings));
        case enkas::simulation::Method::Leapfrog:
            return enkas::simulation::Factory::create(getLeapfrogSettings(settings));
        case enkas::simulation::Method::Hermite:
            return enkas::simulation::Factory::create(getHermiteSettings(settings));
        case enkas::simulation::Method::HITS:
            return enkas::simulation::Factory::create(getHitsSettings(settings));
        case enkas::simulation::Method::BarnesHutLeapfrog:
            return enkas::simulation::Factory::create(getBarnesHutLeapfrogSettings(settings));
        default:
            return nullptr;  // Unsupported generation method
    }
}
