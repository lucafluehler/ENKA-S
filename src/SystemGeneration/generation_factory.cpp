#include "generation_factory.h"

#include "generation_settings.h"
#include "generator.h"

#include "gm_file.h"
#include "gm_normal_sphere.h"
#include "gm_uniform_cube.h"
#include "gm_uniform_sphere.h"
#include "gm_plummer_sphere.h"
#include "gm_spiral_galaxy.h"
#include "gm_collision_model.h"
#include "gm_flyby_model.h"

std::unique_ptr<Generator> GenerationFactory::create(const GenerationSettings& settings)
{
    if (!settings.isValid()) return nullptr;

    switch (settings.method)
    {
    case GenerationMethod::File:
        return std::make_unique<GM_File>(settings.file_settings);
        break;

    case GenerationMethod::NormalSphere:
        return std::make_unique<GM_NormalSphere>(settings.normal_sphere_settings, settings.seed);
        break;

    case GenerationMethod::UniformCube:
        return std::make_unique<GM_UniformCube>(settings.uniform_cube_settings, settings.seed);
        break;

    case GenerationMethod::UniformSphere:
        return std::make_unique<GM_UniformSphere>(settings.uniform_sphere_settings, settings.seed);
        break;

    case GenerationMethod::PlummerSphere:
        return std::make_unique<GM_PlummerSphere>(settings.plummer_sphere_settings, settings.seed);

    case GenerationMethod::SpiralGalaxy:
        return std::make_unique<GM_SpiralGalaxy>(settings.spiral_galaxy_settings, settings.seed);
        break;

    case GenerationMethod::CollisionModel:
        return std::make_unique<GM_CollisionModel>(settings.collision_model_settings, settings.seed);
        break;

    case GenerationMethod::FlybyModel:
        return std::make_unique<GM_FlybyModel>(settings.flyby_model_settings, settings.seed);
        break;

    default:
        break;
    };

    return nullptr;
}
