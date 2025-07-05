#ifndef RENDER_DATA_H_
#define RENDER_DATA_H_

#include <vector>
#include "geometric_algebra.h"

struct RenderData
{
    double time;
    std::vector<ga::Vector3D> positions; // Positions of all particles in system
    ga::Vector3D com_position; // Center of mass position
};

#endif // RENDER_DATA_H_
