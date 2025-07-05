#pragma once

#include "gm_file.h"
#include "gm_normal_sphere.h"
#include "gm_uniform_cube.h"
#include "gm_plummer_sphere.h"
#include "gm_spiral_galaxy.h"
#include "gm_flyby_model.h"
#include "gm_collision_model.h"
#include "gm_uniform_sphere.h"

enum class GenerationMethod
{
    File,
    NormalSphere,
    UniformCube,
    UniformSphere,
    PlummerSphere,
    SpiralGalaxy,
    CollisionModel,
    FlybyModel
};

struct GenerationSettings
{
    GenerationMethod method;
    unsigned int seed;

    GM_File::Settings file_settings;
    GM_NormalSphere::Settings normal_sphere_settings;
    GM_UniformCube::Settings uniform_cube_settings;
    GM_UniformSphere::Settings uniform_sphere_settings;
    GM_PlummerSphere::Settings plummer_sphere_settings;
    GM_SpiralGalaxy::Settings spiral_galaxy_settings;
    GM_CollisionModel::Settings collision_model_settings;
    GM_FlybyModel::Settings flyby_model_settings;

    bool isValid() const
    {
        switch (method) {
        case GenerationMethod::File: return file_settings.isValid(); break;
        case GenerationMethod::NormalSphere: return normal_sphere_settings.isValid(); break;
        case GenerationMethod::UniformCube: return uniform_cube_settings.isValid(); break;
        case GenerationMethod::UniformSphere: return uniform_sphere_settings.isValid(); break;
        case GenerationMethod::PlummerSphere: return plummer_sphere_settings.isValid(); break;
        case GenerationMethod::SpiralGalaxy: return spiral_galaxy_settings.isValid(); break;
        case GenerationMethod::CollisionModel: return collision_model_settings.isValid(); break;
        case GenerationMethod::FlybyModel: return flyby_model_settings.isValid(); break;
        default: break;
        }

        return false;
    }
};
