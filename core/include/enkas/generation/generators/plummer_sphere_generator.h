#pragma once

#include <random>
#include <vector>

#include <enkas/data/system.h>
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
     * @return A System object containing the generated particles.
     */
    [[nodiscard]] data::Systen createSystem() override;

private:
    PlummerSphereSettings settings_;
    unsigned int seed_;
};

} // namespace enkas::generation
