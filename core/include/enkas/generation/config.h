#pragma once

#include <variant>

#include <enkas/generation/file_settings.h>
#include <enkas/generation/normal_sphere_settings.h>
#include <enkas/generation/uniform_cube_settings.h>
#include <enkas/generation/uniform_sphere_settings.h>
#include <enkas/generation/plummer_sphere_settings.h>
#include <enkas/generation/spiral_galaxy_settings.h>
#include <enkas/generation/flyby_model_settings.h>
#include <enkas/generation/collision_model_settings.h>

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
