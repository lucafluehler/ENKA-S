#pragma once

#include "geometric_algebra.h"

struct Camera 
{
    math::Vector3D target_pos;
    float target_distance = 6.0f;
    math::Rotor3D rel_rotation;
};
