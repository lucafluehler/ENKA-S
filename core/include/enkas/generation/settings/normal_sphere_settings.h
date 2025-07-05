#pragma once

namespace enkas::generation {

struct NormalSphereSettings {
    int N;
    double pos_std;
    double vel_std;
    double mass_mean;
    double mass_std;

    [[nodiscard]] bool isValid() const
    {
        return (   N > 0 && pos_std > 0.0 && vel_std > 0.0
                && mass_mean > 0.0 && mass_std > 0.0);
    }
};

} // namespace enkas::generation