#pragma once

#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/collision_model_settings.h>

namespace enkas::generation {

class CollisionModelGenerator : public Generator
{
public:
    explicit CollisionModelGenerator(const CollisionModelSettings& settings, unsigned int seed);

    [[nodiscard]] data::InitialSystem createSystem() override;

private:
    CollisionModelSettings settings;
    unsigned int seed;
};

} // namespace enkas::generation