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
#include <enkas/generation/flyby_model_generator.h>

namespace enkas {
namespace generation {

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
            if constexpr (std::is_same_v<SettingsType, FileSettings>) {
                return std::make_unique<FileGenerator>(specific_settings);
            }
            if constexpr (std::is_same_v<SettingsType, NormalSphereSettings>) {
                return std::make_unique<NormalSphereGenerator>(specific_settings);
            }
            if constexpr (std::is_same_v<SettingsType, UniformCubeSettings>) {
                return std::make_unique<UniformCubeGenerator>(specific_settings);
            }
            if constexpr (std::is_same_v<SettingsType, UniformSphereSettings>) {
                return std::make_unique<UniformSphereGenerator>(specific_settings);
            }
            if constexpr (std::is_same_v<SettingsType, PlummerSphereSettings>) {
                return std::make_unique<PlummerSphereGenerator>(specific_settings);
            }
            if constexpr (std::is_same_v<SettingsType, SpiralGalaxySettings>) {
                return std::make_unique<SpiralGalaxyGenerator>(specific_settings);
            }
            if constexpr (std::is_same_v<SettingsType, CollisionModelSettings>) {
                return std::make_unique<CollisionModelGenerator>(specific_settings);
            }
            if constexpr (std::is_same_v<SettingsType, FlybyModelSettings>) {
                return std::make_unique<FlybyModelGenerator>(specific_settings);
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

} // namespace generation
} // namespace enkas
