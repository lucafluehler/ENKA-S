#pragma once

namespace enkas::generation {

struct UniformSphereSettings {
    int particle_count;
    double sphere_radius;
    double initial_velocity;
    double total_mass;

    [[nodiscard]] bool isValid() const {
        return (particle_count > 0 && sphere_radius > 0.0 && initial_velocity > 0.0 &&
                total_mass > 0.0);
    }
};

}  // namespace enkas::generation
