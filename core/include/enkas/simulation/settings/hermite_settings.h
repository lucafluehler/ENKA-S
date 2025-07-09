#pragma once

namespace enkas::simulation {

struct HermiteSettings {
    double time_step;
    double softening_parameter;

    [[nodiscard]] bool isValid() const { return (time_step != 0.0 && softening_parameter != 0.0); }
};

}  // namespace enkas::simulation
