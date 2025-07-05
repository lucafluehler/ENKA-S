#ifndef UTILS_H_
#define UTILS_H_

#include "geometric_algebra.h"

namespace utils {

struct CenterOfMass
{
    ga::Vector3D pos;
    ga::Vector3D vel;
};

} // namespace utils

#endif // UTILS_H_