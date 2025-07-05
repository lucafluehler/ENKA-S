#pragma once

#include <cmath>

#include <enkas/math/vector3d.h>

namespace enkas {
namespace math {

class Bivector3D {
public:
    double xy;
    double xz;
    double yz;

    Bivector3D(double xy = 0.0, double xz = 0.0, double yz = 0.0);

public:
    Bivector3D operator+(const Bivector3D& rhs) const;
    Bivector3D operator+=(const Bivector3D& rhs);

public:
    /**
     * @brief Calculates the squared norm of the Bivector
     */
    double norm2() const;

    /**
     * @brief Calculates the norm of the Bivector
     */
    double norm() const;

    /**
     * @brief Calculates the vector perpendicular to the plane represented by the bivector.
     *
     * This function computes the Hodge dual of the bivector to find the perpendicular vector
     * to the plane represented by the bivector in 3D space.
     */
    Vector3D getPerpendicular() const;

public:
    static Bivector3D XY(double xy_val = 1.0) { return Bivector3D(xy_val, 0.0, 0.0); }
    static Bivector3D XZ(double xz_val = 1.0) { return Bivector3D(0.0, xz_val, 0.0); }
    static Bivector3D YZ(double yz_val = 1.0) { return Bivector3D(0.0, 0.0, yz_val); }
};

} // namespace math
} // namespace enkas
