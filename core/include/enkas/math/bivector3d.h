#pragma once

#include <enkas/math/vector3d.h>

namespace enkas::math {

class Bivector3D {
   public:
    double xy = 0.0;
    double xz = 0.0;
    double yz = 0.0;

    Bivector3D() = default;
    Bivector3D(double p_xy, double p_xz, double p_yz);

    Bivector3D(const Bivector3D& rhs) = default;
    Bivector3D& operator=(const Bivector3D& rhs) = default;
    Bivector3D(Bivector3D&& rhs) noexcept = default;
    Bivector3D& operator=(Bivector3D&& rhs) noexcept = default;

   public:  // Compound assignment operators
    Bivector3D& operator+=(const Bivector3D& rhs);
    Bivector3D& operator-=(const Bivector3D& rhs);
    Bivector3D& operator*=(double scalar);
    Bivector3D& operator/=(double scalar);

   public:
    /**
     * @brief Calculates the squared norm of the Bivector
     */
    [[nodiscard]] double norm2() const;

    /**
     * @brief Calculates the norm of the Bivector
     */
    [[nodiscard]] double norm() const;

    /**
     * @brief Calculates the vector perpendicular to the plane represented by the bivector.
     *
     * This function computes the Hodge dual of the bivector to find the perpendicular vector
     * to the plane represented by the bivector in 3D space.
     */
    [[nodiscard]] Vector3D getPerpendicular() const;

   public:
    /**
     * @brief Creates a Bivector3D parallel to the xy-plane with a specified xy value.
     */
    static Bivector3D XY(double xy_val = 1.0) { return Bivector3D(xy_val, 0.0, 0.0); }

    /**
     * @brief Creates a Bivector3D parallel to the xz-plane with a specified xz value.
     */
    static Bivector3D XZ(double xz_val = 1.0) { return Bivector3D(0.0, xz_val, 0.0); }

    /**
     * @brief Creates a Bivector3D parallel to the yz-plane with a specified yz value.
     */
    static Bivector3D YZ(double yz_val = 1.0) { return Bivector3D(0.0, 0.0, yz_val); }
};

// Binary operators

inline Bivector3D operator+(Bivector3D lhs, const Bivector3D& rhs) {
    lhs += rhs;
    return lhs;
}

inline Bivector3D operator-(Bivector3D lhs, const Bivector3D& rhs) {
    lhs -= rhs;
    return lhs;
}

inline Bivector3D operator*(Bivector3D lhs, double scalar) {
    lhs *= scalar;
    return lhs;
}

inline Bivector3D operator*(double scalar, Bivector3D rhs) {
    rhs *= scalar;
    return rhs;
}

inline Bivector3D operator/(Bivector3D lhs, double scalar) {
    lhs /= scalar;
    return lhs;
}

/**
 * @brief Calculates the wedge (exterior) product of two Vector3D objects.
 *
 * The wedge product of two vectors `lhs` and `rhs` results in a Bivector3D representing
 * the oriented area spanned by the parallelogram defined by the vectors.
 *
 * @param lhs The left-hand Vector3D.
 * @param rhs The right-hand Vector3D.
 *
 * @return A Bivector3D representing the wedge product of the input vectors.
 */
inline Bivector3D wedge(const Vector3D& lhs, const Vector3D& rhs) {
    return Bivector3D(lhs.x * rhs.y - lhs.y * rhs.x,
                      lhs.x * rhs.z - lhs.z * rhs.x,
                      lhs.y * rhs.z - lhs.z * rhs.y);
}

}  // namespace enkas::math
