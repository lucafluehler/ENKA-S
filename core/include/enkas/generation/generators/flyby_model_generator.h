#pragma once

#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/flyby_model_settings.h>

namespace enkas {
namespace generation {

class FlybyModelGenerator : public Generator
{
public:
    explicit FlybyModelGenerator(const FlybyModelSettings& settings, unsigned int seed);

    [[nodiscard]] vdata::InitialSystem createSystem() override;

private:
    FlybyModelSettings settings;
    unsigned int seed;
};

} // namespace generation
} // namespace enkas
