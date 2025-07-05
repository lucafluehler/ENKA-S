#pragma once

#include <variant>

#include <enkas/generation/settings/file_settings.h>
#include <enkas/generation/settings/normal_sphere_settings.h>
#include <enkas/generation/settings/uniform_cube_settings.h>
#include <enkas/generation/settings/uniform_sphere_settings.h>
#include <enkas/generation/settings/plummer_sphere_settings.h>
#include <enkas/generation/settings/spiral_galaxy_settings.h>
#include <enkas/generation/settings/flyby_model_settings.h>
#include <enkas/generation/settings/collision_model_settings.h>

namespace enkas {
namespace generation {

using SettingsVariant = std::variant<
    FileSettings,
    NormalSphereSettings,
    UniformCubeSettings,
    UniformSphereSettings,
    PlummerSphereSettings,
    SpiralGalaxySettings,
    CollisionModelSettings,
    FlybyModelSettings
>;

struct GenerationConfig {
    unsigned int seed;
    SettingsVariant specific_settings;

    [[nodiscard]] bool isValid() const {
        return std::visit([](const auto& settings) {
            return settings.isValid();
        }, specific_settings);
    }
};

} // namespace generation
} // namespace enkas
