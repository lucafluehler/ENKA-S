#pragma once

#include <enkas/math/vector3d.h>

#include <numbers>
#include <random>

namespace enkas::math {

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
inline math::Vector3D getRandOnSphere(std::mt19937& gen, double norm = 1.0) noexcept {
    static thread_local std::uniform_real_distribution<double> dist_z(-1.0, 1.0);
    static thread_local std::uniform_real_distribution<double> dist_phi(0.0, 2 * std::numbers::pi);

    const double z = dist_z(gen);
    const double phi = dist_phi(gen);
    const double r = std::sqrt(1.0 - z * z);

    return Vector3D{r * std::cos(phi), r * std::sin(phi), z} * norm;
}

}  // namespace enkas::math
