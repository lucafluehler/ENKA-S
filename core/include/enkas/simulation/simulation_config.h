#pragma once

#include <variant>

#include <enkas/simulation/settings/euler_settings.h>
#include <enkas/simulation/settings/leapfrog_settings.h>
#include <enkas/simulation/settings/hermite_settings.h>
#include <enkas/simulation/settings/hits_settings.h>
#include <enkas/simulation/settings/bhleapfrog_settings.h>

namespace enkas::simulation {

using SettingsVariant = std::variant<
    EulerSettings,
    LeapfrogSettings,
    HermiteSettings,
    HitsSettings,
    BhLeapfrogSettings
>;

struct SimulationConfig {
    double duration;
    SettingsVariant specific_settings;

    [[nodiscard]] bool isValid() const {
        return std::visit([](const auto& settings) {
            return settings.isValid();
        }, specific_settings);
    }
};

} // namespace enkas::simulation
