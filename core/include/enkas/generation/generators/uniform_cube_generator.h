#pragma once

#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/uniform_cube_settings.h>

namespace enkas::generation {

class UniformCubeGenerator : public Generator
{
public:
    UniformCubeGenerator(const UniformCubeSettings& settings, unsigned int seed);

    [[nodiscard]] data::InitialSystem createSystem() override;

private:
    UniformCubeSettings settings;
    unsigned int seed;
};

} // namespace enkas::generation
