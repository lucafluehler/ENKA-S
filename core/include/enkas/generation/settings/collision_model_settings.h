#pragma once

namespace enkas::generation {

struct CollisionModelSettings {
    double impact_parameter;
    double relative_velocity;

    int particle_count_1;
    double sphere_radius_1;
    double total_mass_1;

    int particle_count_2;
    double sphere_radius_2;
    double total_mass_2;

    [[nodiscard]] bool isValid() const {
        return (particle_count_1 > 0 && sphere_radius_1 > 0.0 && total_mass_1 > 0.0 &&
                particle_count_2 > 0 && sphere_radius_2 > 0.0 && total_mass_2 > 0.0);
    }
};

}  // namespace enkas::generation
