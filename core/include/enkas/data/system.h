#pragma once

#include <vector>

#include <enkas/math/vector3d.h>

namespace enkas::data {

struct System {
    std::vector<math::Vector3D> positions;
    std::vector<math::Vector3D> velocities;
    std::vector<double> masses;

    size_t count() const { return positions.size(); }
};

} // namespace enkas::data
