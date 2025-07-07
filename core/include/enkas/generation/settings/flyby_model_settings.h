#pragma once

namespace enkas::generation {

struct FlybyModelSettings {
    int particle_count;
    double sphere_radius;
    double total_mass;

    double body_mass;

    [[nodiscard]] bool isValid() const {
        return (
            particle_count > 0 &&
            sphere_radius > 0.0 &&
            total_mass > 0.0 &&
            body_mass > 0.0
        );
    }
};

} // namespace enkas::generation
