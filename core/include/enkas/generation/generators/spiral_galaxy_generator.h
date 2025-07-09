#pragma once

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/spiral_galaxy_settings.h>

namespace enkas::generation {

class SpiralGalaxyGenerator : public Generator {
public:
    explicit SpiralGalaxyGenerator(const SpiralGalaxySettings& settings, unsigned int seed);

    [[nodiscard]] data::System createSystem() override;

private:
    SpiralGalaxySettings settings_;
    unsigned int seed_;
};

}  // namespace enkas::generation
