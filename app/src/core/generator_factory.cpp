#include "core/generator_factory.h"

#include <enkas/generation/generation_factory.h>

#include <memory>

std::unique_ptr<enkas::generation::Generator> GeneratorFactory::create(const Settings& settings) {
    auto method_str = settings.get<std::string>("GenerationMethod");
    auto method = enkas::generation::stringToMethod(method_str).value();

    switch (method) {
        case enkas::generation::Method::NormalSphere:
            return enkas::generation::Factory::create(getNormalSphereSettings(settings));
        case enkas::generation::Method::UniformCube:
            return enkas::generation::Factory::create(getUniformCubeSettings(settings));
        case enkas::generation::Method::UniformSphere:
            return enkas::generation::Factory::create(getUniformSphereSettings(settings));
        case enkas::generation::Method::PlummerSphere:
            return enkas::generation::Factory::create(getPlummerSphereSettings(settings));
        case enkas::generation::Method::SpiralGalaxy:
            return enkas::generation::Factory::create(getSpiralGalaxySettings(settings));
        case enkas::generation::Method::CollisionModel:
            return enkas::generation::Factory::create(getCollisionModelSettings(settings));
        default:
            return nullptr;  // Unsupported generation method
    }
}
