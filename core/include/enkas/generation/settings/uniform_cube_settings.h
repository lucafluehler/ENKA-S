#pragma once

namespace enkas::generation {

struct UniformCubeSettings {
    int N;
    double side_length;
    double vel;
    double total_mass;

    bool isValid() const
    {
        return ( N > 0 && side_length > 0.0 && vel > 0.0 && total_mass > 0.0);
    }
};

} // namespace enkas::generation