#pragma once

namespace enkas::generation {

struct UniformCubeSettings {
    int particle_count;
    double side_length;
    double initial_velocity;
    double total_mass;

    [[nodiscard]] bool isValid() const {
        return ( 
            particle_count > 0 && 
            side_length > 0.0 && 
            initial_velocity > 0.0 && 
            total_mass > 0.0
        );
    }
};

} // namespace enkas::generation
