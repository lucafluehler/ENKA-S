#pragma once

#include <vector>

#include "generator.h"
#include "utils.h"

class GM_FlybyModel : public Generator
{
public:
    struct Settings
    {
        int N;
        double radius;
        double total_mass;

        double body_mass;

        bool isValid() const
        {
            return (   N > 0 && radius > 0.0 && total_mass > 0.0
                    && body_mass > 0.0 );
        }
    };

public:
    GM_FlybyModel(const Settings& settings, unsigned int seed);

    utils::InitialSystem createSystem() override;

private:
    Settings settings;
    unsigned int seed;
};
