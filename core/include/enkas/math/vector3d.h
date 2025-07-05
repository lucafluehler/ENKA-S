#pragma once

#include <cmath>

namespace enkas::math {

class Vector3D {
public:
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vector3D() = default;
    Vector3D(double p_x, double p_y, double p_z);

    Vector3D(const Vector3D& rhs) = default;
    Vector3D& operator=(const Vector3D& rhs) = default;
    Vector3D(Vector3D&& rhs) noexcept = default;
    Vector3D& operator=(Vector3D&& rhs) noexcept = default;

public: // Compound assignment operators
    Vector3D& operator+=(const Vector3D& rhs);
    Vector3D& operator-=(const Vector3D& rhs);
    Vector3D& operator*=(double rhs);
    Vector3D& operator/=(double rhs);

public: // Utility functions
    /**
     * @brief Calculates the squared norm of the Vector3D
     */
    [[nodiscard]] double norm2() const;

    /**
     * @brief Calculates the norm of the Vector3D
     */
    [[nodiscard]] double norm() const;

    /**
     * @brief Set the norm of the 3D vector to a specified value.
     *
     * This function scales the vector to have the specified Euclidean norm in place.
     * If the current norm is not zero, it rescales the vector to the desired norm.
     * If the current norm is zero, the vector remains unchanged.
     *
     * @param new_norm The new Euclidean norm to set for the vector.
     *
     * @return A reference to the modified 3D vector.
     */
    Vector3D& set_norm(double new_norm);

    /**
     * @brief Fills all components of the vector with a specified value.
     */
    void fill(double value);

public:
    /**
     * @brief Creates a Vector3D parallel to the x-axis with a specified x value.
     */
    static Vector3D X(double x_val = 1.0) { return Vector3D(x_val, 0.0, 0.0); }

    /**
     * @brief Creates a Vector3D parallel to the y-axis with a specified y value.
     */
    static Vector3D Y(double y_val = 1.0) { return Vector3D(0.0, y_val, 0.0); }
    
    /**
     * @brief Creates a Vector3D parallel to the z-axis with a specified z value.
     */
    static Vector3D Z(double z_val = 1.0) { return Vector3D(0.0, 0.0, z_val); }
};

// Binary operators
inline Vector3D operator+(Vector3D lhs, const Vector3D& rhs) {
    lhs += rhs;
    return lhs;
}

inline Vector3D operator-(Vector3D lhs, const Vector3D& rhs) {
    lhs -= rhs;
    return lhs;
}

inline Vector3D operator*(Vector3D lhs, double rhs) {
    lhs *= rhs;
    return lhs;
}

inline Vector3D operator*(double lhs, Vector3D rhs) {
    rhs *= lhs;
    return rhs;
}

inline Vector3D operator/(Vector3D lhs, double rhs) {
    lhs /= rhs;
    return lhs;
}

/**
 * @brief Calculates the dot product of two Vector3D objects.
 *
 * @param lhs The left-hand Vector3D operand.
 * @param rhs The right-hand Vector3D operand.
 */
inline double dotProduct(const Vector3D& lhs, const Vector3D& rhs)
{
    return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
}

} // namespace enkas::math
