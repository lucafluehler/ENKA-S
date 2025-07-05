#pragma once

#include <vector>

#include "generator.h"
#include "utils.h"

class GM_UniformCube : public Generator
{
public:
    struct Settings
    {
        int N;
        double side_length;
        double vel;
        double total_mass;

        bool isValid() const
        {
            return ( N > 0 && side_length > 0.0 && vel > 0.0 && total_mass > 0.0);
        }
    };

public:
    GM_UniformCube(const Settings& settings, unsigned int seed);

    utils::InitialSystem createSystem() override;

private:
    Settings settings;
    unsigned int seed;
};
