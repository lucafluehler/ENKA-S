#include "core/generator_factory.h"

#include <enkas/generation/generation_factory.h>

#include <memory>

#include "core/settings/settings.h"

std::unique_ptr<enkas::generation::Generator> GeneratorFactory::create(const Settings& settings) {
    auto method = settings.get<GenerationMethod>(SettingKey::GenerationMethod);

    switch (method) {
        case GenerationMethod::NormalSphere:
            return enkas::generation::Factory::create(getNormalSphereSettings(settings));
        case GenerationMethod::UniformCube:
            return enkas::generation::Factory::create(getUniformCubeSettings(settings));
        case GenerationMethod::UniformSphere:
            return enkas::generation::Factory::create(getUniformSphereSettings(settings));
        case GenerationMethod::PlummerSphere:
            return enkas::generation::Factory::create(getPlummerSphereSettings(settings));
        case GenerationMethod::SpiralGalaxy:
            return enkas::generation::Factory::create(getSpiralGalaxySettings(settings));
        case GenerationMethod::CollisionModel:
            return enkas::generation::Factory::create(getCollisionModelSettings(settings));
        case GenerationMethod::File:
        default:
            return nullptr;  // Unsupported generation method
    }
}

enkas::generation::NormalSphereSettings GeneratorFactory::getNormalSphereSettings(
    const Settings& settings) {
    enkas::generation::NormalSphereSettings normal_sphere_settings;
    normal_sphere_settings.seed = settings.get<int>(SettingKey::NormalSphereSeed);
    normal_sphere_settings.particle_count =
        settings.get<int>(SettingKey::NormalSphereParticleCount);
    normal_sphere_settings.position_std_dev =
        settings.get<double>(SettingKey::NormalSpherePositionStdDev);
    normal_sphere_settings.velocity_std_dev =
        settings.get<double>(SettingKey::NormalSphereVelocityStdDev);
    normal_sphere_settings.mass_mean = settings.get<double>(SettingKey::NormalSphereMassMean);
    normal_sphere_settings.mass_std_dev = settings.get<double>(SettingKey::NormalSphereMassStdDev);
    return normal_sphere_settings;
}

enkas::generation::UniformCubeSettings GeneratorFactory::getUniformCubeSettings(
    const Settings& settings) {
    enkas::generation::UniformCubeSettings uniform_cube_settings;
    uniform_cube_settings.seed = settings.get<int>(SettingKey::UniformCubeSeed);
    uniform_cube_settings.particle_count = settings.get<int>(SettingKey::UniformCubeParticleCount);
    uniform_cube_settings.side_length = settings.get<double>(SettingKey::UniformCubeSideLength);
    uniform_cube_settings.initial_velocity =
        settings.get<double>(SettingKey::UniformCubeInitialVelocity);
    uniform_cube_settings.total_mass = settings.get<double>(SettingKey::UniformCubeTotalMass);
    return uniform_cube_settings;
}

enkas::generation::UniformSphereSettings GeneratorFactory::getUniformSphereSettings(
    const Settings& settings) {
    enkas::generation::UniformSphereSettings uniform_sphere_settings;
    uniform_sphere_settings.seed = settings.get<int>(SettingKey::UniformSphereSeed);
    uniform_sphere_settings.particle_count =
        settings.get<int>(SettingKey::UniformSphereParticleCount);
    uniform_sphere_settings.sphere_radius = settings.get<double>(SettingKey::UniformSphereRadius);
    uniform_sphere_settings.initial_velocity =
        settings.get<double>(SettingKey::UniformSphereInitialVelocity);
    uniform_sphere_settings.total_mass = settings.get<double>(SettingKey::UniformSphereTotalMass);
    return uniform_sphere_settings;
}

enkas::generation::PlummerSphereSettings GeneratorFactory::getPlummerSphereSettings(
    const Settings& settings) {
    enkas::generation::PlummerSphereSettings plummer_sphere_settings;
    plummer_sphere_settings.seed = settings.get<int>(SettingKey::PlummerSphereSeed);
    plummer_sphere_settings.particle_count =
        settings.get<int>(SettingKey::PlummerSphereParticleCount);
    plummer_sphere_settings.sphere_radius = settings.get<double>(SettingKey::PlummerSphereRadius);
    plummer_sphere_settings.total_mass = settings.get<double>(SettingKey::PlummerSphereTotalMass);
    return plummer_sphere_settings;
}

enkas::generation::SpiralGalaxySettings GeneratorFactory::getSpiralGalaxySettings(
    const Settings& settings) {
    enkas::generation::SpiralGalaxySettings spiral_galaxy_settings;
    spiral_galaxy_settings.seed = settings.get<int>(SettingKey::SpiralGalaxySeed);
    spiral_galaxy_settings.particle_count =
        settings.get<int>(SettingKey::SpiralGalaxyParticleCount);
    spiral_galaxy_settings.num_arms = settings.get<int>(SettingKey::SpiralGalaxyNumArms);
    spiral_galaxy_settings.radius = settings.get<double>(SettingKey::SpiralGalaxyRadius);
    spiral_galaxy_settings.total_mass = settings.get<double>(SettingKey::SpiralGalaxyTotalMass);
    spiral_galaxy_settings.twist = settings.get<double>(SettingKey::SpiralGalaxyTwist);
    spiral_galaxy_settings.black_hole_mass =
        settings.get<double>(SettingKey::SpiralGalaxyBlackHoleMass);
    return spiral_galaxy_settings;
}

enkas::generation::CollisionModelSettings GeneratorFactory::getCollisionModelSettings(
    const Settings& settings) {
    enkas::generation::CollisionModelSettings collision_model_settings;
    collision_model_settings.seed = settings.get<int>(SettingKey::CollisionModelSeed);
    collision_model_settings.particle_count_1 =
        settings.get<int>(SettingKey::CollisionModelParticleCount1);
    collision_model_settings.sphere_radius_1 =
        settings.get<double>(SettingKey::CollisionModelSphereRadius1);
    collision_model_settings.total_mass_1 =
        settings.get<double>(SettingKey::CollisionModelTotalMass1);
    collision_model_settings.particle_count_2 =
        settings.get<int>(SettingKey::CollisionModelParticleCount2);
    collision_model_settings.sphere_radius_2 =
        settings.get<double>(SettingKey::CollisionModelSphereRadius2);
    collision_model_settings.total_mass_2 =
        settings.get<double>(SettingKey::CollisionModelTotalMass2);
    return collision_model_settings;
}
