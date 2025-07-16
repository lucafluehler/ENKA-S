#pragma once

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/uniform_cube_settings.h>

namespace enkas::generation {

class UniformCubeGenerator : public Generator {
public:
    UniformCubeGenerator(const UniformCubeSettings& settings);

    [[nodiscard]] data::System createSystem() override;

private:
    UniformCubeSettings settings_;
};

}  // namespace enkas::generation
