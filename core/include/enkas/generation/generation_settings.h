#include <enkas/generation/settings/collision_model_settings.h>
#include <enkas/generation/settings/normal_sphere_settings.h>
#include <enkas/generation/settings/plummer_sphere_settings.h>
#include <enkas/generation/settings/spiral_galaxy_settings.h>
#include <enkas/generation/settings/uniform_cube_settings.h>
#include <enkas/generation/settings/uniform_sphere_settings.h>

#include <variant>

namespace enkas::generation {

using Settings = std::variant<NormalSphereSettings,
                              UniformCubeSettings,
                              UniformSphereSettings,
                              PlummerSphereSettings,
                              SpiralGalaxySettings,
                              CollisionModelSettings>;

}  // namespace enkas::generation
