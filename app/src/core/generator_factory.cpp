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
    enkas::generation::NormalSphereSettings out;
    out.seed = settings.get<int>(SettingKey::NormalSphereSeed);
    out.particle_count = settings.get<int>(SettingKey::NormalSphereParticleCount);
    out.position_std_dev = settings.get<double>(SettingKey::NormalSpherePositionStdDev);
    out.velocity_std_dev = settings.get<double>(SettingKey::NormalSphereVelocityStdDev);
    out.mass_mean = settings.get<double>(SettingKey::NormalSphereMassMean);
    out.mass_std_dev = settings.get<double>(SettingKey::NormalSphereMassStdDev);
    return out;
}

enkas::generation::UniformCubeSettings GeneratorFactory::getUniformCubeSettings(
    const Settings& settings) {
    enkas::generation::UniformCubeSettings out;
    out.seed = settings.get<int>(SettingKey::UniformCubeSeed);
    out.particle_count = settings.get<int>(SettingKey::UniformCubeParticleCount);
    out.side_length = settings.get<double>(SettingKey::UniformCubeSideLength);
    out.initial_velocity = settings.get<double>(SettingKey::UniformCubeInitialVelocity);
    out.total_mass = settings.get<double>(SettingKey::UniformCubeTotalMass);
    return out;
}

enkas::generation::UniformSphereSettings GeneratorFactory::getUniformSphereSettings(
    const Settings& settings) {
    enkas::generation::UniformSphereSettings out;
    out.seed = settings.get<int>(SettingKey::UniformSphereSeed);
    out.particle_count = settings.get<int>(SettingKey::UniformSphereParticleCount);
    out.sphere_radius = settings.get<double>(SettingKey::UniformSphereRadius);
    out.initial_velocity = settings.get<double>(SettingKey::UniformSphereInitialVelocity);
    out.total_mass = settings.get<double>(SettingKey::UniformSphereTotalMass);
    return out;
}

enkas::generation::PlummerSphereSettings GeneratorFactory::getPlummerSphereSettings(
    const Settings& settings) {
    enkas::generation::PlummerSphereSettings out;
    out.seed = settings.get<int>(SettingKey::PlummerSphereSeed);
    out.particle_count = settings.get<int>(SettingKey::PlummerSphereParticleCount);
    out.sphere_radius = settings.get<double>(SettingKey::PlummerSphereRadius);
    out.total_mass = settings.get<double>(SettingKey::PlummerSphereTotalMass);
    return out;
}

enkas::generation::SpiralGalaxySettings GeneratorFactory::getSpiralGalaxySettings(
    const Settings& settings) {
    enkas::generation::SpiralGalaxySettings out;
    out.seed = settings.get<int>(SettingKey::SpiralGalaxySeed);
    out.particle_count = settings.get<int>(SettingKey::SpiralGalaxyParticleCount);
    out.num_arms = settings.get<int>(SettingKey::SpiralGalaxyNumArms);
    out.radius = settings.get<double>(SettingKey::SpiralGalaxyRadius);
    out.total_mass = settings.get<double>(SettingKey::SpiralGalaxyTotalMass);
    out.twist = settings.get<double>(SettingKey::SpiralGalaxyTwist);
    out.black_hole_mass = settings.get<double>(SettingKey::SpiralGalaxyBlackHoleMass);
    return out;
}

enkas::generation::CollisionModelSettings GeneratorFactory::getCollisionModelSettings(
    const Settings& settings) {
    enkas::generation::CollisionModelSettings out;
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
