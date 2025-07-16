#include <enkas/generation/generation_factory.h>
#include <enkas/generation/generation_settings.h>
#include <enkas/generation/generators/collision_model_generator.h>
#include <enkas/generation/generators/normal_sphere_generator.h>
#include <enkas/generation/generators/plummer_sphere_generator.h>
#include <enkas/generation/generators/spiral_galaxy_generator.h>
#include <enkas/generation/generators/uniform_cube_generator.h>
#include <enkas/generation/generators/uniform_sphere_generator.h>
#include <enkas/logging/logger.h>

#include <memory>
#include <type_traits>
#include <variant>

namespace enkas::generation {

std::unique_ptr<Generator> Factory::create(const Settings& settings) {
    ENKAS_LOG_INFO("Attempting to create generator from settings...");

    if (!std::visit([](const auto& s) { return s.isValid(); }, settings)) {
        ENKAS_LOG_ERROR("Settings are invalid. Cannot create generator.");
        return nullptr;
    }

    return std::visit(
        [](const auto& specific_settings) -> std::unique_ptr<Generator> {
            using SettingsType = std::decay_t<decltype(specific_settings)>;

            if constexpr (std::is_same_v<SettingsType, NormalSphereSettings>) {
                return std::make_unique<NormalSphereGenerator>(specific_settings);
            } else if constexpr (std::is_same_v<SettingsType, UniformCubeSettings>) {
                return std::make_unique<UniformCubeGenerator>(specific_settings);
            } else if constexpr (std::is_same_v<SettingsType, UniformSphereSettings>) {
                return std::make_unique<UniformSphereGenerator>(specific_settings);
            } else if constexpr (std::is_same_v<SettingsType, PlummerSphereSettings>) {
                return std::make_unique<PlummerSphereGenerator>(specific_settings);
            } else if constexpr (std::is_same_v<SettingsType, SpiralGalaxySettings>) {
                return std::make_unique<SpiralGalaxyGenerator>(specific_settings);
            } else if constexpr (std::is_same_v<SettingsType, CollisionModelSettings>) {
                return std::make_unique<CollisionModelGenerator>(specific_settings);
            } else {
                ENKAS_LOG_CRITICAL("Unhandled settings type '{}' in Factory. No generator created.",
                                   typeid(SettingsType).name());
                return nullptr;
            }
        },
        settings);
}

}  // namespace enkas::generation
