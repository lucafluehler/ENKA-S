#pragma once

#include <enkas/data/base_particle.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/normal_sphere_settings.h>

namespace enkas::generation {

class NormalSphereGenerator : public Generator
{
public:
    explicit NormalSphereGenerator(const NormalSphereSettings& settings, unsigned int seed);

    [[nodiscard]] InitialSystem createSystem() override;

private:
    NormalSphereSettings settings;
    unsigned int seed;
};

} // namespace enkas::generation
