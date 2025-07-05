#pragma once

#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/uniform_sphere_settings.h>

class UniformSphereGenerator : public Generator
{
public:
    explicit UniformSphereGenerator(const NormalSphereSettings& settings, unsigned int seed);

    [[nodiscard]] data::InitialSystem createSystem() override;

private:
    UniformSphereSettings settings;
    unsigned int seed;
};
