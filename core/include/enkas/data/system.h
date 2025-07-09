#pragma once

#include <enkas/math/vector3d.h>

#include <vector>


namespace enkas::data {

struct System {
    std::vector<math::Vector3D> positions;
    std::vector<math::Vector3D> velocities;
    std::vector<double> masses;

    /**
     * @brief Returns the number of particles in the system.
     */
    [[nodiscard]] size_t count() const noexcept { return positions.size(); }

    /**
     * @brief Resizes the system to contain 'n' particles.
     *
     * This function resizes the positions, velocities, and masses vectors
     * to ensure they all have the same size of 'n'. If 'n' is smaller than
     * the current size, excess elements will be removed.
     *
     * @param n The new size for the system.
     */
    void resize(size_t n) {
        positions.resize(n);
        velocities.resize(n);
        masses.resize(n);
    }
};

}  // namespace enkas::data
