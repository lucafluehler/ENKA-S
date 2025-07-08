#pragma once

namespace enkas::simulation {

struct HitsSettings {
    double time_step_parameter = 0.0;
    double softening_parameter = 0.0;

    [[nodiscard]] bool isValid() const {
        return (
            time_step_parameter != 0.0 && 
            softening_parameter != 0.0
        );
    }
};

} // namespace enkas::simulation
