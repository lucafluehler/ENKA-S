#pragma once

namespace enkas {
namespace generation {

struct FlybyModelSettings
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

} // namespace generation
} // namespace enkas