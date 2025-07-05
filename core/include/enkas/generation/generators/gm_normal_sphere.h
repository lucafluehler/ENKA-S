#pragma once

#include <vector>

#include "generator.h"
#include "utils.h"

class GM_NormalSphere : public Generator
{
public:
    struct Settings
    {
        int N;
        double pos_std;
        double vel_std;
        double mass_mean;
        double mass_std;

        bool isValid() const
        {
            return (   N > 0 && pos_std > 0.0 && vel_std > 0.0
                    && mass_mean > 0.0 && mass_std > 0.0);
        }
    };

public:
    GM_NormalSphere(const Settings& settings, unsigned int seed);

    utils::InitialSystem createSystem() override;

private:
    Settings settings;
    unsigned int seed;
};
