#pragma oncev

#include <vector>
#include "geometric_algebra.h"

struct RenderData
{
    double time;
    std::vector<ga::Vector3D> positions; // Positions of all particles in system
    ga::Vector3D com_position; // Center of mass position
};
