#pragma once

#include <vector>

#include <enkas/math/vector3d.h>

namespace enkas::data {

struct BaseParticle
{
    Vector3D pos{};
    Vector3D vel{};
    double mass = 0.0;
};

} // namespace enkas::data
