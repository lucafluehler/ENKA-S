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

enkas::simulation::EulerSettings SimulatorFactory::getEulerSettings(const Settings& settings) {
    enkas::simulation::EulerSettings out;
    out.time_step = settings.get<double>(SettingKey::EulerTimeStep);
    out.softening_parameter = settings.get<double>(SettingKey::EulerSoftening);
    return out;
}

enkas::simulation::LeapfrogSettings SimulatorFactory::getLeapfrogSettings(
    const Settings& settings) {
    enkas::simulation::LeapfrogSettings out;
    out.time_step = settings.get<double>(SettingKey::LeapfrogTimeStep);
    out.softening_parameter = settings.get<double>(SettingKey::LeapfrogSoftening);
    return out;
}

enkas::simulation::HermiteSettings SimulatorFactory::getHermiteSettings(const Settings& settings) {
    enkas::simulation::HermiteSettings out;
    out.time_step = settings.get<double>(SettingKey::HermiteTimeStep);
    out.softening_parameter = settings.get<double>(SettingKey::HermiteSoftening);
    return out;
}

enkas::simulation::HitsSettings SimulatorFactory::getHitsSettings(const Settings& settings) {
    enkas::simulation::HitsSettings out;
    out.time_step_parameter = settings.get<double>(SettingKey::HitsTimeStepParam);
    out.softening_parameter = settings.get<double>(SettingKey::HitsSoftening);
    return out;
}

enkas::simulation::BarnesHutLeapfrogSettings SimulatorFactory::getBarnesHutLeapfrogSettings(
    const Settings& settings) {
    enkas::simulation::BarnesHutLeapfrogSettings out;
    out.time_step = settings.get<double>(SettingKey::BarnesHutLeapfrogTimeStep);
    out.theta_mac = settings.get<double>(SettingKey::BarnesHutLeapfrogThetaMac);
    out.softening_parameter = settings.get<double>(SettingKey::BarnesHutLeapfrogSoftening);
    return out;
}
