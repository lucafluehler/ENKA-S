#include "core/generator_factory.h"

#include <enkas/generation/generation_factory.h>
#include <enkas/generation/generator.h>
#include <enkas/logging/logger.h>

#include <memory>

#include "core/settings/settings.h"

using Generator = enkas::generation::Generator;
using Factory = enkas::generation::Factory;
using UniformCubeSettings = enkas::generation::UniformCubeSettings;
using NormalSphereSettings = enkas::generation::NormalSphereSettings;
using UniformSphereSettings = enkas::generation::UniformSphereSettings;
using PlummerSphereSettings = enkas::generation::PlummerSphereSettings;
using SpiralGalaxySettings = enkas::generation::SpiralGalaxySettings;
using CollisionModelSettings = enkas::generation::CollisionModelSettings;

std::unique_ptr<Generator> GeneratorFactory::create(const Settings& settings) {
    try {
        auto method = settings.get<GenerationMethod>(SettingKey::GenerationMethod);

        switch (method) {
            case GenerationMethod::NormalSphere:
                return Factory::create(getNormalSphereSettings(settings));
            case GenerationMethod::UniformCube:
                return Factory::create(getUniformCubeSettings(settings));
            case GenerationMethod::UniformSphere:
                return Factory::create(getUniformSphereSettings(settings));
            case GenerationMethod::PlummerSphere:
                return Factory::create(getPlummerSphereSettings(settings));
            case GenerationMethod::SpiralGalaxy:
                return Factory::create(getSpiralGalaxySettings(settings));
            case GenerationMethod::CollisionModel:
                return Factory::create(getCollisionModelSettings(settings));
            case GenerationMethod::File:
            default:
                ENKAS_LOG_ERROR("Unsupported generation method: {}",
                                std::string(generationMethodToString(method)));
                return nullptr;  // Unsupported generation method
        }
    } catch (const std::exception& e) {
        ENKAS_LOG_ERROR("Error occurred while creating generator: {}", e.what());
        return nullptr;  // An expected key was not found in the provided settings
    }
}

NormalSphereSettings GeneratorFactory::getNormalSphereSettings(const Settings& settings) {
    NormalSphereSettings out;
    out.seed = settings.get<int>(SettingKey::NormalSphereSeed);
    out.particle_count = settings.get<int>(SettingKey::NormalSphereParticleCount);
    out.position_std_dev = settings.get<double>(SettingKey::NormalSpherePositionStdDev);
    out.velocity_std_dev = settings.get<double>(SettingKey::NormalSphereVelocityStdDev);
    out.mass_mean = settings.get<double>(SettingKey::NormalSphereMassMean);
    out.mass_std_dev = settings.get<double>(SettingKey::NormalSphereMassStdDev);
    return out;
}

UniformCubeSettings GeneratorFactory::getUniformCubeSettings(const Settings& settings) {
    UniformCubeSettings out;
    out.seed = settings.get<int>(SettingKey::UniformCubeSeed);
    out.particle_count = settings.get<int>(SettingKey::UniformCubeParticleCount);
    out.side_length = settings.get<double>(SettingKey::UniformCubeSideLength);
    out.initial_velocity = settings.get<double>(SettingKey::UniformCubeInitialVelocity);
    out.total_mass = settings.get<double>(SettingKey::UniformCubeTotalMass);
    return out;
}

UniformSphereSettings GeneratorFactory::getUniformSphereSettings(const Settings& settings) {
    UniformSphereSettings out;
    out.seed = settings.get<int>(SettingKey::UniformSphereSeed);
    out.particle_count = settings.get<int>(SettingKey::UniformSphereParticleCount);
    out.sphere_radius = settings.get<double>(SettingKey::UniformSphereRadius);
    out.initial_velocity = settings.get<double>(SettingKey::UniformSphereInitialVelocity);
    out.total_mass = settings.get<double>(SettingKey::UniformSphereTotalMass);
    return out;
}

PlummerSphereSettings GeneratorFactory::getPlummerSphereSettings(const Settings& settings) {
    PlummerSphereSettings out;
    out.seed = settings.get<int>(SettingKey::PlummerSphereSeed);
    out.particle_count = settings.get<int>(SettingKey::PlummerSphereParticleCount);
    out.sphere_radius = settings.get<double>(SettingKey::PlummerSphereRadius);
    out.total_mass = settings.get<double>(SettingKey::PlummerSphereTotalMass);
    return out;
}

SpiralGalaxySettings GeneratorFactory::getSpiralGalaxySettings(const Settings& settings) {
    SpiralGalaxySettings out;
    out.seed = settings.get<int>(SettingKey::SpiralGalaxySeed);
    out.particle_count = settings.get<int>(SettingKey::SpiralGalaxyParticleCount);
    out.num_arms = settings.get<int>(SettingKey::SpiralGalaxyNumArms);
    out.radius = settings.get<double>(SettingKey::SpiralGalaxyRadius);
    out.total_mass = settings.get<double>(SettingKey::SpiralGalaxyTotalMass);
    out.twist = settings.get<double>(SettingKey::SpiralGalaxyTwist);
    out.black_hole_mass = settings.get<double>(SettingKey::SpiralGalaxyBlackHoleMass);
    return out;
}

CollisionModelSettings GeneratorFactory::getCollisionModelSettings(const Settings& settings) {
    CollisionModelSettings out;
    out.seed = settings.get<int>(SettingKey::CollisionModelSeed);
    out.impact_parameter = settings.get<double>(SettingKey::CollisionModelImpactParameter);
    out.relative_velocity = settings.get<double>(SettingKey::CollisionModelRelativeVelocity);
    out.particle_count_1 = settings.get<int>(SettingKey::CollisionModelParticleCount1);
    out.sphere_radius_1 = settings.get<double>(SettingKey::CollisionModelSphereRadius1);
    out.total_mass_1 = settings.get<double>(SettingKey::CollisionModelTotalMass1);
    out.particle_count_2 = settings.get<int>(SettingKey::CollisionModelParticleCount2);
    out.sphere_radius_2 = settings.get<double>(SettingKey::CollisionModelSphereRadius2);
    out.total_mass_2 = settings.get<double>(SettingKey::CollisionModelTotalMass2);
    return out;
}
