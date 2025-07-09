#pragma once

namespace enkas::generation {

struct NormalSphereSettings {
    int particle_count;
    double position_std_dev;
    double velocity_std_dev;
    double mass_mean;
    double mass_std_dev;

    [[nodiscard]] bool isValid() const {
        return (particle_count > 0 && position_std_dev > 0.0 && velocity_std_dev > 0.0 &&
                mass_mean > 0.0 && mass_std_dev > 0.0);
    }
};

}  // namespace enkas::generation
