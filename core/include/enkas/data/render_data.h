#pragma oncev

#include <vector>
#include "geometric_algebra.h"

struct RenderData
{
    double time;
    std::vector<math::Vector3D> positions; // Positions of all particles in system
    math::Vector3D com_position; // Center of mass position
};
