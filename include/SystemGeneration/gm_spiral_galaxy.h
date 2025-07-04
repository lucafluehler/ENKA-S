#ifndef GM_SPIRAL_GALAXY_H_
#define GM_SPIRAL_GALAXY_H_

#include <vector>

#include "generator.h"
#include "utils.h"

class GM_SpiralGalaxy : public Generator
{
public:
    struct Settings
    {
        int N;
        int arms;
        double radius;
        double total_mass;
        double twist;

        double bh_mass;

        bool isValid() const
        {
            return (   N > 1 && arms > 0 && radius > 0.0 && total_mass > 0.0
                    && twist > 0.0 && bh_mass > 0.0 );
        }
    };

public:
    GM_SpiralGalaxy(const Settings& settings, unsigned int seed);

    utils::InitialSystem createSystem() override;

private:
    Settings settings;
    unsigned int seed;
};

#endif // GM_SPIRAL_GALAXY_H_
