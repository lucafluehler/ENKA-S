#pragma once

namespace enkas::generation {

struct UniformSphereSettings
    {
        int N;
        double radius;
        double vel;
        double total_mass;

        bool isValid() const
        {
            return ( N > 0 && radius > 0.0 && vel > 0.0 && total_mass > 0.0);
        }
    };

} // namespace enkas::generation