#pragma once

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/uniform_sphere_settings.h>

namespace enkas::generation {

class UniformSphereGenerator : public Generator {
   public:
    explicit UniformSphereGenerator(const UniformSphereSettings& settings, unsigned int seed);

    [[nodiscard]] data::System createSystem() override;

   private:
    UniformSphereSettings settings_;
    unsigned int seed_;
};

}  // namespace enkas::generation
