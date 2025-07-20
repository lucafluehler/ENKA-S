#pragma once

#include <enkas/generation/generation_factory.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/collision_model_settings.h>
#include <enkas/generation/settings/normal_sphere_settings.h>
#include <enkas/generation/settings/plummer_sphere_settings.h>
#include <enkas/generation/settings/spiral_galaxy_settings.h>
#include <enkas/generation/settings/uniform_cube_settings.h>
#include <enkas/generation/settings/uniform_sphere_settings.h>

#include <memory>

#include "core/settings/settings.h"

/**
 * @brief Factory class for creating different types of generators based on settings.
 */
class GeneratorFactory {
    using Generator = enkas::generation::Generator;
    using UniformCubeSettings = enkas::generation::UniformCubeSettings;
    using NormalSphereSettings = enkas::generation::NormalSphereSettings;
    using UniformSphereSettings = enkas::generation::UniformSphereSettings;
    using PlummerSphereSettings = enkas::generation::PlummerSphereSettings;
    using SpiralGalaxySettings = enkas::generation::SpiralGalaxySettings;
    using CollisionModelSettings = enkas::generation::CollisionModelSettings;

public:
    /**
     * @brief Creates a generator based on the provided settings.
     * Relies on SettingKey::GenerationMethod to determine the type of generator.
     * @param settings The settings to configure the generator.
     * @return A unique pointer to the created generator. If the settings do not match any known
     * generator type, returns nullptr.
     */
    static std::unique_ptr<Generator> create(const Settings& settings);

private:
    static UniformCubeSettings getUniformCubeSettings(const Settings& settings);
    static NormalSphereSettings getNormalSphereSettings(const Settings& settings);
    static UniformSphereSettings getUniformSphereSettings(const Settings& settings);
    static PlummerSphereSettings getPlummerSphereSettings(const Settings& settings);
    static SpiralGalaxySettings getSpiralGalaxySettings(const Settings& settings);
    static CollisionModelSettings getCollisionModelSettings(const Settings& settings);
};
