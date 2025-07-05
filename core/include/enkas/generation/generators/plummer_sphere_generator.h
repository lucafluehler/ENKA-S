#pragma once

#include <random>
#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generator.h>
#include <enkas/generation/settings/plummer_sphere_settings.h>

namespace enkas::generation {

class PlummerSphereGenerator : public Generator
{
public:
    explicit PlummerSphereGenerator(const PlummerSphereSettings& settings, unsigned int seed);

    /**
     * @brief Generates a system of particles following the Plummer sphere distribution.
     *
     * Uses the algorithm provided by Aarseth et. al. in Astronomy and Astrophysics,
     * vol. 37, no. 1, Dec. 1974, p. 183-187.
     *
     * @return A vector of BaseParticle objects representing the generated particle system.
     */
    [[nodiscard]] data::InitialSystem createSystem() override;

private:
    PlummerSphereSettings settings;
    unsigned int seed;
};

} // namespace enkas::generation
