#include <enkas/simulation/simulation_config.h>
#include <enkas/simulation/simulation_factory.h>
#include <enkas/simulation/simulator.h>
#include <enkas/simulation/simulators/barneshutleapfrog_simulator.h>
#include <enkas/simulation/simulators/euler_simulator.h>
#include <enkas/simulation/simulators/hermite_simulator.h>
#include <enkas/simulation/simulators/hits_simulator.h>
#include <enkas/simulation/simulators/leapfrog_simulator.h>

namespace enkas::simulation {

std::shared_ptr<Simulator> SimulationFactory::create(const SimulationConfig& config) {
    if (!config.isValid()) {
        return nullptr;
    }

    return std::visit(
        [&config](const auto& specific_settings) -> std::shared_ptr<Simulator> {
            // Get the concrete type of the settings object we were passed.
            using SettingsType = std::decay_t<decltype(specific_settings)>;

            // Depending on the type of settings, create the corresponding simulator.
            if constexpr (std::is_same_v<SettingsType, EulerSettings>) {
                return std::make_shared<EulerSimulator>(specific_settings);
            }
            if constexpr (std::is_same_v<SettingsType, LeapfrogSettings>) {
                return std::make_shared<LeapfrogSimulator>(specific_settings);
            }
            if constexpr (std::is_same_v<SettingsType, HermiteSettings>) {
                return std::make_shared<HermiteSimulator>(specific_settings);
            }
            if constexpr (std::is_same_v<SettingsType, HitsSettings>) {
                return std::make_shared<HitsSimulator>(specific_settings);
            }
            if constexpr (std::is_same_v<SettingsType, BarnesHutLeapfrogSettings>) {
                return std::make_shared<BarnesHutLeapfrogSimulator>(specific_settings);
            } else {
                return nullptr;
            }
        },
        config.specific_settings);
}

}  // namespace enkas::simulation
