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
