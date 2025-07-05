#pragma once

namespace enkas::generation {

struct CollisionModelsSettings {
    int N_1;
    double radius_1;
    double total_mass_1;

    int N_2;
    double radius_2;
    double total_mass_2;

    bool isValid() const
    {
        return (   N_1 > 0 && radius_1 > 0.0 && total_mass_1 > 0.0
                && N_2 > 0 && radius_2 > 0.0 && total_mass_2 > 0.0 );
    }
};

} // namespace enkas::generation