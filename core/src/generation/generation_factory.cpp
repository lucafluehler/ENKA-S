#include <enkas/generation/generation_config.h>
#include <enkas/generation/generation_factory.h>
#include <enkas/generation/generators/collision_model_generator.h>
#include <enkas/generation/generators/normal_sphere_generator.h>
#include <enkas/generation/generators/plummer_sphere_generator.h>
#include <enkas/generation/generators/spiral_galaxy_generator.h>
#include <enkas/generation/generators/stream_generator.h>
#include <enkas/generation/generators/uniform_cube_generator.h>
#include <enkas/generation/generators/uniform_sphere_generator.h>
#include <enkas/logging/logger.h>

#include <iostream>
#include <type_traits>
#include <variant>

namespace enkas::generation {

std::unique_ptr<Generator> GenerationFactory::create(const GenerationConfig& config) {
    ENKAS_LOG_INFO("Attempting to create generator from config...");

    if (!config.isValid()) {
        ENKAS_LOG_ERROR("GenerationConfig is invalid. Cannot create generator.");
        return nullptr;
    }

    ENKAS_LOG_DEBUG("Using random seed: {}", config.seed);

    return std::visit(
        [&config](const auto& specific_settings) -> std::unique_ptr<Generator> {
            // Get the concrete type of the settings object we were passed.
            using SettingsType = std::decay_t<decltype(specific_settings)>;

            // Depending on the type of settings, create the corresponding generator.
            if constexpr (std::is_same_v<SettingsType, NormalSphereSettings>) {
                return std::make_unique<NormalSphereGenerator>(specific_settings, config.seed);
            }
            if constexpr (std::is_same_v<SettingsType, UniformCubeSettings>) {
                return std::make_unique<UniformCubeGenerator>(specific_settings, config.seed);
            }
            if constexpr (std::is_same_v<SettingsType, UniformSphereSettings>) {
                return std::make_unique<UniformSphereGenerator>(specific_settings, config.seed);
            }
            if constexpr (std::is_same_v<SettingsType, PlummerSphereSettings>) {
                return std::make_unique<PlummerSphereGenerator>(specific_settings, config.seed);
            }
            if constexpr (std::is_same_v<SettingsType, SpiralGalaxySettings>) {
                return std::make_unique<SpiralGalaxyGenerator>(specific_settings, config.seed);
            }
            if constexpr (std::is_same_v<SettingsType, CollisionModelSettings>) {
                return std::make_unique<CollisionModelGenerator>(specific_settings, config.seed);
            } else {
                // Development error: if we reach here, it means we have an unsupported settings
                // type. This should never happen if the settings are properly defined.
                ENKAS_LOG_CRITICAL(
                    "Unhandled settings type '{}' in GenerationFactory. No generator created.",
                    typeid(SettingsType).name());
                return nullptr;
            }
        },
        config.specific_settings);
}

std::unique_ptr<Generator> GenerationFactory::create(std::istream& stream) {
    ENKAS_LOG_INFO("Attempting to create generator from input stream...");

    if (!stream) {
        ENKAS_LOG_ERROR("Input stream is invalid. Cannot create generator.");
        return nullptr;
    }

    return std::make_unique<StreamGenerator>(stream);
}

}  // namespace enkas::generation
