#pragma once

#include <enkas/simulation/settings/barneshutleapfrog_settings.h>
#include <enkas/simulation/settings/euler_settings.h>
#include <enkas/simulation/settings/hermite_settings.h>
#include <enkas/simulation/settings/hits_settings.h>
#include <enkas/simulation/settings/leapfrog_settings.h>

#include <variant>

namespace enkas::simulation {

using Settings = std::variant<EulerSettings,
                              LeapfrogSettings,
                              HermiteSettings,
                              HitsSettings,
                              BarnesHutLeapfrogSettings>;

}  // namespace enkas::simulation
