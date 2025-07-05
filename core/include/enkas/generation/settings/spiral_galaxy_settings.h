#pragma once

namespace enkas {
namespace generation {

struct SpiralGalaxySettings{
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

} // namespace generation
} // namespace enkas