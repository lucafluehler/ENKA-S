#pragma once

#include <random>

#include <enkas/math/vector3d.h>

namespace enkas::math {
    constexpr double Pi = 3.14159265358979323846;

/**
 * @brief Generate a random 3D vector on the surface of a sphere.
 *
 * This function generates a random 3D vector where the direction is uniformly distributed
 * on the surface of a unit sphere and then scales it by the provided 'norm' value.
 *
 * @param norm The desired magnitude (norm) of the generated vector.
 * @return A Vector3D representing a random vector on the sphere's surface, scaled by 'norm'.
 *
 * @note The function uses a random number generator to create the random vector.
 */
inline math::Vector3D getRandOnSphere(std::mt19937& gen, double norm = 1.0)
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    math::Vector3D sphere;

    const double theta = std::acos(2.0*dist(gen) - 1.0);
    const double phi = 2.0*Pi*dist(gen);

    sphere.x = std::sin(theta)*std::cos(phi);
    sphere.y = std::sin(theta)*std::sin(phi);
    sphere.z = std::cos(theta);

    return sphere*norm;
}

} // namespace enkas::math
