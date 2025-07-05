#pragma once

#include <cmath>

namespace enkas {
namespace math {

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
    static Vector3D X(double x_val = 1.0) { return Vector3D(x_val, 0.0, 0.0); }
    static Vector3D Y(double y_val = 1.0) { return Vector3D(0.0, y_val, 0.0); }
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

// Allow for `2.0 * my_vector`
inline Vector3D operator*(double lhs, Vector3D rhs) {
    rhs *= lhs;
    return rhs;
}

inline Vector3D operator/(Vector3D lhs, double rhs) {
    lhs /= rhs;
    return lhs;
}

} // namespace math
} // namespace enkas
