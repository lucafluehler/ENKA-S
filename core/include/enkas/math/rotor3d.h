#pragma once

#include <enkas/math/bivector3d.h>
#include <enkas/math/vector3d.h>

namespace enkas::math {

class Rotor3D {
   public:
    double s = 1.0;
    double b_xy = 0.0;
    double b_xz = 0.0;
    double b_yz = 0.0;

    Rotor3D() = default;
    Rotor3D(double p_s, double p_b_xy, double p_b_xz, double p_b_yz);
    Rotor3D(double angle_rad, const Bivector3D& plane);

    Rotor3D(const Rotor3D& rhs) = default;
    Rotor3D& operator=(const Rotor3D& rhs) = default;
    Rotor3D(Rotor3D&& rhs) noexcept = default;
    Rotor3D& operator=(Rotor3D&& rhs) noexcept = default;

   public:  // Compound assignment operators
    Rotor3D& operator*=(const Rotor3D& rhs);

   public:
    /**
     * @brief Calculates the squared norm of the Rotor3D
     */
    [[nodiscard]] double norm2() const;

    /**
     * @brief Calculates the norm of the Rotor3D
     */
    [[nodiscard]] double norm() const;

    /**
     * @brief Rotates a 3D vector using this Rotor3D.
     *
     * @param vec The input 3D vector to be rotated.
     *
     * @return The rotated 3D vector.
     */
    [[nodiscard]] Vector3D rotate(const Vector3D& vec) const;

    /**
     * @brief Normalizes the Rotor3D, making it have a unit length.
     */
    Rotor3D& normalize();

    /**
     * @brief Get the reverse of this Rotor3D.
     */
    [[nodiscard]] Rotor3D get_reverse() const;

   public:
    /**
     * @brief Creates a Rotor3D over the xy-plane with a specified xy value.
     */
    static Rotor3D XY(double xy_val = 1.0) { return Rotor3D(1.0, xy_val, 0.0, 0.0); }

    /**
     * @brief Creates a Rotor3D over the xz-plane with a specified xz value.
     */
    static Rotor3D XZ(double xz_val = 1.0) { return Rotor3D(1.0, 0.0, xz_val, 0.0); }

    /**
     * @brief Creates a Rotor3D over the yz-plane with a specified yz value.
     */
    static Rotor3D YZ(double yz_val = 1.0) { return Rotor3D(1.0, 0.0, 0.0, yz_val); }
};

// Binary operator

inline Rotor3D operator*(Rotor3D lhs, const Rotor3D& rhs) {
    lhs *= rhs;
    return lhs;
}

}  // namespace enkas::math
