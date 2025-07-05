#pragma once

namespace enkas::data {

struct AnalyticsData
{
    double time;

    // Lagrange Radii
    double mass_radius_10 = 0.0; // 10 % mass radius
    double mass_radius_25 = 0.0; // 25 % mass radius
    double half_mass_radius = 0.0;
    double mass_radius_75 = 0.0; // 75 % mass radius
    double mass_radius_90 = 0.0; // 90 % mass radius

    double t_rh; // half-mass relaxation time
    double t_dh; // dynamical time
};

} // namespace enkas::data
