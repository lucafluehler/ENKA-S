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

namespace enkas::simulation {

std::unique_ptr<Simulator> Factory::create(const Settings& settings) {
    ENKAS_LOG_INFO("Attempting to create simulator from settings...");

    if (!std::visit([](const auto& s) { return s.isValid(); }, settings)) {
        ENKAS_LOG_ERROR("Settings are invalid. Cannot create simulator.");
        return nullptr;
    }

    return std::visit(
        [&settings](const auto& specific_settings) -> std::unique_ptr<Simulator> {
            using SettingsType = std::decay_t<decltype(specific_settings)>;

            if constexpr (std::is_same_v<SettingsType, EulerSettings>) {
                return std::make_unique<EulerSimulator>(specific_settings);
            } else if constexpr (std::is_same_v<SettingsType, LeapfrogSettings>) {
                return std::make_unique<LeapfrogSimulator>(specific_settings);
            } else if constexpr (std::is_same_v<SettingsType, HermiteSettings>) {
                return std::make_unique<HermiteSimulator>(specific_settings);
            } else if constexpr (std::is_same_v<SettingsType, HitsSettings>) {
                return std::make_unique<HitsSimulator>(specific_settings);
            } else if constexpr (std::is_same_v<SettingsType, BarnesHutLeapfrogSettings>) {
                return std::make_unique<BarnesHutLeapfrogSimulator>(specific_settings);
            } else {
                // Development error: if we reach here, it means we have an unsupported settings
                // type. This should never happen if the settings are properly defined.
                ENKAS_LOG_CRITICAL("Unhandled settings type '{}' in Factory. No simulator created.",
                                   typeid(SettingsType).name());
                return nullptr;
            }
        },
        settings);
}

}  // namespace enkas::simulation
