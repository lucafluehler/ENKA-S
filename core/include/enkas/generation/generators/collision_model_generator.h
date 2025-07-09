#pragma once

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/collision_model_settings.h>

namespace enkas::generation {

class CollisionModelGenerator : public Generator {
   public:
    explicit CollisionModelGenerator(const CollisionModelSettings& settings, unsigned int seed);

    [[nodiscard]] data::System createSystem() override;

   private:
    CollisionModelSettings settings_;
    unsigned int seed_;
};

}  // namespace enkas::generation