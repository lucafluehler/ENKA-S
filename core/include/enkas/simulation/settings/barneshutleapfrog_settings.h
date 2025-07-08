#pragma once

namespace enkas::simulation {

struct BarnesHutLeapfrogSettings {
    double time_step;
    double theta_mac; // multipole acceptance criterion
    double softening_parameter;

    [[nodiscard]] bool isValid() const {
        return (
            time_step > 0.0 && 
            theta_mac >= 0.0 &&
            softening_parameter > 0.0
        );
    }
};

} // namespace enkas::simulation
