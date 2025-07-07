#pragma once

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/normal_sphere_settings.h>

namespace enkas::generation {

class NormalSphereGenerator : public Generator
{
public:
    explicit NormalSphereGenerator(const NormalSphereSettings& settings, unsigned int seed);

    [[nodiscard]] data::System createSystem() override;

private:
    NormalSphereSettings settings_;
    unsigned int seed_;
};

} // namespace enkas::generation
