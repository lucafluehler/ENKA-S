#include <variant>
#include <type_traits>
#include <iostream>

#include <enkas/generation/generation_factory.h>

#include <enkas/generation/config.h>

#include <enkas/generation/stream_generator.h>
#include <enkas/generation/normal_sphere_generator.h>
#include <enkas/generation/uniform_cube_generator.h>
#include <enkas/generation/uniform_sphere_generator.h>
#include <enkas/generation/plummer_sphere_generator.h>
#include <enkas/generation/spiral_galaxy_generator.h>
#include <enkas/generation/collision_model_generator.h>
s
namespace enkas::generation {

std::unique_ptr<Generator> GenerationFactory::create(const GenerationConfig& config)
{
    if (!config.isValid()) {
        return nullptr;
    }

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
            }
            else {
                return nullptr;
            }
        },
        config.specific_settings
    );
}

std::unique_ptr<Generator> GenerationFactory::create(std::istream& stream)
{
    if (!stream) {
        return nullptr;
    }

    return std::make_unique<StreamGenerator>(stream);
}

} // namespace enkas::generation
