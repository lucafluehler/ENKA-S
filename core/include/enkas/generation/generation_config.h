#pragma once

#include <enkas/generation/settings/collision_model_settings.h>
#include <enkas/generation/settings/normal_sphere_settings.h>
#include <enkas/generation/settings/plummer_sphere_settings.h>
#include <enkas/generation/settings/spiral_galaxy_settings.h>
#include <enkas/generation/settings/uniform_cube_settings.h>
#include <enkas/generation/settings/uniform_sphere_settings.h>

#include <variant>


namespace enkas::generation {

using SettingsVariant =
    std::variant<NormalSphereSettings, UniformCubeSettings, UniformSphereSettings,
                 PlummerSphereSettings, SpiralGalaxySettings, CollisionModelSettings>;

struct GenerationConfig {
    unsigned int seed;
    SettingsVariant specific_settings;

    [[nodiscard]] bool isValid() const {
        return std::visit([](const auto& settings) { return settings.isValid(); },
                          specific_settings);
    }
};

}  // namespace enkas::generation
