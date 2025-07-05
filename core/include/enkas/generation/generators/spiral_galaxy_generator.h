#pragma once

#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/spiral_galaxy_settings.h>

namespace enkas::generation {

class SpiralGalaxyGenerator : public Generator
{
public:
    explicit SpiralGalaxyGenerator(const SpiralGalaxySettings& settings, unsigned int seed);

    [[nodiscard]] data::InitialSystem createSystem() override;

private:
    SpiralGalaxySettings settings;
    unsigned int seed;
};

} // namespace enkas::generation
