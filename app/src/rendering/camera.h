#pragma once

#include <enkas/math/rotor3d.h>
#include <enkas/math/vector3d.h>

struct Camera {
    enkas::math::Vector3D target_pos;
    float target_distance = 6.0f;
    enkas::math::Rotor3D rel_rotation;
};
