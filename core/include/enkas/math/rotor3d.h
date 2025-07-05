#pragma once

#include <cmath>

#include <enkas/math/vector3d.h>
#include <enkas/math/bivector3d.h>

namespace enkas {
namespace math {

class Rotor3D {
public:
    double s;
    double b_xy;
    double b_xz;
    double b_yz;

    Rotor3D(double s = 1.0, double b_xy = 0.0, double b_xz = 0.0, double b_yz = 0.0);
    Rotor3D(double angle, const Bivector3D& plane);

public:
    Rotor3D operator*(const Rotor3D& rhs) const;

public:
    /**
     * @brief Calculates the squared norm of the Rotor3D
     */
    double norm2() const;

    /**
     * @brief Calculates the norm of the Rotor3D
     */
    double norm() const;

    /**
     * @brief Rotates a 3D vector using this Rotor3D.
     *
     * @param vec The input 3D vector to be rotated.
     *
     * @return The rotated 3D vector.
     */
    Vector3D rotate(const Vector3D& vec) const;

    /**
     * @brief Normalizes the Rotor3D, making it have a unit length.
     */
    Rotor3D normalize();

    /**
     * @brief Get the reverse of this Rotor3D.
     */
    Rotor3D get_reverse() const;

public:
    static Rotor3D XY(double xy_val = 1.0) { return Rotor3D(1.0, xy_val, 0.0, 0.0); }
    static Rotor3D XZ(double xz_val = 1.0) { return Rotor3D(1.0, 0.0, xz_val, 0.0); }
    static Rotor3D YZ(double yz_val = 1.0) { return Rotor3D(1.0, 0.0, 0.0, yz_val); }
};

} // namespace math
} // namespace enkas
