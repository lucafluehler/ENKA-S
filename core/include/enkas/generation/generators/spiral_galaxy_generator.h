#pragma once

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/spiral_galaxy_settings.h>

namespace enkas::generation {

class SpiralGalaxyGenerator : public Generator {
public:
    explicit SpiralGalaxyGenerator(const SpiralGalaxySettings& settings);

    [[nodiscard]] data::System createSystem() override;

private:
    SpiralGalaxySettings settings_;
};

}  // namespace enkas::generation
