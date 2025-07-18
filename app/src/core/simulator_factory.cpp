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
    enkas::simulation::EulerSettings euler_settings;
    euler_settings.time_step = settings.get<double>(SettingKey::EulerTimeStep);
    euler_settings.softening_parameter = settings.get<double>(SettingKey::EulerSoftening);
    return euler_settings;
}

enkas::simulation::LeapfrogSettings SimulatorFactory::getLeapfrogSettings(
    const Settings& settings) {
    enkas::simulation::LeapfrogSettings leapfrog_settings;
    leapfrog_settings.time_step = settings.get<double>(SettingKey::LeapfrogTimeStep);
    leapfrog_settings.softening_parameter = settings.get<double>(SettingKey::LeapfrogSoftening);
    return leapfrog_settings;
}

enkas::simulation::HermiteSettings SimulatorFactory::getHermiteSettings(const Settings& settings) {
    enkas::simulation::HermiteSettings hermite_settings;
    hermite_settings.time_step = settings.get<double>(SettingKey::HermiteTimeStep);
    hermite_settings.softening_parameter = settings.get<double>(SettingKey::HermiteSoftening);
    return hermite_settings;
}

enkas::simulation::HitsSettings SimulatorFactory::getHitsSettings(const Settings& settings) {
    enkas::simulation::HitsSettings hits_settings;
    hits_settings.time_step_parameter = settings.get<double>(SettingKey::HitsTimeStepParam);
    hits_settings.softening_parameter = settings.get<double>(SettingKey::HitsSoftening);
    return hits_settings;
}

enkas::simulation::BarnesHutLeapfrogSettings SimulatorFactory::getBarnesHutLeapfrogSettings(
    const Settings& settings) {
    enkas::simulation::BarnesHutLeapfrogSettings bhleapfrog_settings;
    bhleapfrog_settings.time_step = settings.get<double>(SettingKey::BarnesHutLeapfrogTimeStep);
    bhleapfrog_settings.theta_mac = settings.get<double>(SettingKey::BarnesHutLeapfrogThetaMac);
    bhleapfrog_settings.softening_parameter =
        settings.get<double>(SettingKey::BarnesHutLeapfrogSoftening);
    return bhleapfrog_settings;
}
