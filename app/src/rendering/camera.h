#pragma once

#include "geometric_algebra.h"

struct Camera 
{
    ga::Vector3D target_pos;
    float target_distance = 6.0f;
    ga::Rotor3D rel_rotation;
};
