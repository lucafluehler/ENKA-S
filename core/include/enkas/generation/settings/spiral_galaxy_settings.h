#pragma once

namespace enkas::generation {

struct SpiralGalaxySettings {
    int particle_count;
    int num_arms;
    double radius;
    double total_mass;
    double twist;

    double black_hole_mass;

    [[nodiscard]] bool isValid() const {
        return (particle_count > 0 && num_arms > 0 && radius > 0.0 && total_mass > 0.0 &&
                twist > 0.0 && black_hole_mass > 0.0);
    }
};

}  // namespace enkas::generation
