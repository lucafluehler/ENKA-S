#pragma once

#include <random>
#include <vector>

#include "generator.h"
#include "utils.h"

class GM_PlummerSphere : public Generator
{
public:
    struct Settings
    {
        int N;
        double radius;
        double total_mass;

        bool isValid() const
        {
            return ( N > 0 && radius > 0.0 && total_mass > 0.0);
        }
    };

public:
    GM_PlummerSphere(const Settings& settings, unsigned int seed);

    /**
     * @brief Generates a system of particles following the Plummer sphere distribution.
     *
     * Uses the algorithm provided by Aarseth et. al. in Astronomy and Astrophysics,
     * vol. 37, no. 1, Dec. 1974, p. 183-187.
     *
     * @return A vector of BaseParticle objects representing the generated particle system.
     */
    utils::InitialSystem createSystem() override;

private:
    Settings settings;
    unsigned int seed;
};
