#pragma once

#include <enkas/generation/generation_factory.h>
#include <enkas/generation/generation_method.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/collision_model_settings.h>
#include <enkas/generation/settings/normal_sphere_settings.h>
#include <enkas/generation/settings/plummer_sphere_settings.h>
#include <enkas/generation/settings/spiral_galaxy_settings.h>
#include <enkas/generation/settings/uniform_cube_settings.h>
#include <enkas/generation/settings/uniform_sphere_settings.h>

#include <memory>

#include "core/settings.h"

class GeneratorFactory {
public:
    static std::unique_ptr<enkas::generation::Generator> create(const Settings& settings);

private:
    static enkas::generation::UniformCubeSettings getUniformCubeSettings(const Settings& settings);
    static enkas::generation::NormalSphereSettings getNormalSphereSettings(
        const Settings& settings);
    static enkas::generation::UniformSphereSettings getUniformSphereSettings(
        const Settings& settings);
    static enkas::generation::PlummerSphereSettings getPlummerSphereSettings(
        const Settings& settings);
    static enkas::generation::SpiralGalaxySettings getSpiralGalaxySettings(
        const Settings& settings);
    static enkas::generation::CollisionModelSettings getCollisionModelSettings(
        const Settings& settings);
};