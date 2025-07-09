#pragma once

#include <enkas/simulation/settings/barneshutleapfrog_settings.h>
#include <enkas/simulation/settings/euler_settings.h>
#include <enkas/simulation/settings/hermite_settings.h>
#include <enkas/simulation/settings/hits_settings.h>
#include <enkas/simulation/settings/leapfrog_settings.h>

#include <variant>

namespace enkas::simulation {

using SettingsVariant = std::variant<EulerSettings,
                                     LeapfrogSettings,
                                     HermiteSettings,
                                     HitsSettings,
                                     BarnesHutLeapfrogSettings>;

struct SimulationConfig {
    double duration;
    SettingsVariant specific_settings;

    [[nodiscard]] bool isValid() const {
        return std::visit([](const auto& settings) { return settings.isValid(); },
                          specific_settings);
    }
};

}  // namespace enkas::simulation
