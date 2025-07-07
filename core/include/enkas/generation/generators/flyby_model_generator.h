#pragma once

#include <vector>

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/flyby_model_settings.h>

namespace enkas::generation {

class FlybyModelGenerator : public Generator
{
public:
    explicit FlybyModelGenerator(const FlybyModelSettings& settings, unsigned int seed);

    [[nodiscard]] data::System createSystem() override;

private:
    FlybyModelSettings settings_;
    unsigned int seed_;
};

} // namespace enkas::generation
