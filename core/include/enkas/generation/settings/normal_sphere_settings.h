#pragma once

namespace enkas::generation {

struct NormalSphereSettings {
    int particle_count;
    double pos_std_dev;
    double vel_std_dev;
    double mass_mean;
    double mass_std_dev;

    [[nodiscard]] bool isValid() const{
        return (
            particle_count > 0 &&
            pos_std_dev > 0.0 &&
            vel_std_dev > 0.0 &&
            mass_mean > 0.0 &&
            mass_std_dev > 0.0
        );
    }
};

} // namespace enkas::generation
