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

using Simulator = enkas::simulation::Simulator;
using Factory = enkas::simulation::Factory;
using EulerSettings = enkas::simulation::EulerSettings;
using LeapfrogSettings = enkas::simulation::LeapfrogSettings;
using HermiteSettings = enkas::simulation::HermiteSettings;
using HitsSettings = enkas::simulation::HitsSettings;
using BarnesHutLeapfrogSettings = enkas::simulation::BarnesHutLeapfrogSettings;

std::unique_ptr<Simulator> SimulatorFactory::create(const Settings& settings) {
    try {
        auto method = settings.get<SimulationMethod>(SettingKey::SimulationMethod);

        switch (method) {
            case SimulationMethod::Euler:
                return Factory::create(getEulerSettings(settings));
            case SimulationMethod::Leapfrog:
                return Factory::create(getLeapfrogSettings(settings));
            case SimulationMethod::Hermite:
                return Factory::create(getHermiteSettings(settings));
            case SimulationMethod::Hits:
                return Factory::create(getHitsSettings(settings));
            case SimulationMethod::BarnesHutLeapfrog:
                return Factory::create(getBarnesHutLeapfrogSettings(settings));
            default:
                ENKAS_LOG_ERROR("Unsupported simulation method: {}",
                                std::string(simulationMethodToString(method)));
                return nullptr;  // Unsupported simulation method
        }
    } catch (const std::exception& e) {
        ENKAS_LOG_ERROR("Error occurred while creating simulator: {}", e.what());
        return nullptr;  // An expected key was not found in the provided settings
    }
}

EulerSettings SimulatorFactory::getEulerSettings(const Settings& settings) {
    EulerSettings out;
    out.time_step = settings.get<double>(SettingKey::EulerTimeStep);
    out.softening_parameter = settings.get<double>(SettingKey::EulerSoftening);
    return out;
}

LeapfrogSettings SimulatorFactory::getLeapfrogSettings(const Settings& settings) {
    LeapfrogSettings out;
    out.time_step = settings.get<double>(SettingKey::LeapfrogTimeStep);
    out.softening_parameter = settings.get<double>(SettingKey::LeapfrogSoftening);
    return out;
}

HermiteSettings SimulatorFactory::getHermiteSettings(const Settings& settings) {
    HermiteSettings out;
    out.time_step = settings.get<double>(SettingKey::HermiteTimeStep);
    out.softening_parameter = settings.get<double>(SettingKey::HermiteSoftening);
    return out;
}

HitsSettings SimulatorFactory::getHitsSettings(const Settings& settings) {
    HitsSettings out;
    out.time_step_parameter = settings.get<double>(SettingKey::HitsTimeStepParam);
    out.softening_parameter = settings.get<double>(SettingKey::HitsSoftening);
    return out;
}

BarnesHutLeapfrogSettings SimulatorFactory::getBarnesHutLeapfrogSettings(const Settings& settings) {
    BarnesHutLeapfrogSettings out;
    out.time_step = settings.get<double>(SettingKey::BarnesHutLeapfrogTimeStep);
    out.theta_mac = settings.get<double>(SettingKey::BarnesHutLeapfrogThetaMac);
    out.softening_parameter = settings.get<double>(SettingKey::BarnesHutLeapfrogSoftening);
    return out;
}
