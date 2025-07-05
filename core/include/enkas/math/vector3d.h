#pragma once

#include <cmath>

namespace geometricAlgebra {

class Vector3D {
public:
    double x;
    double y;
    double z;

    Vector3D(double p_x = 0.0, double p_y = 0.0, double p_z = 0.0);
    Vector3D(const Vector3D& rhs);

public: // operators
    // Vector addition and subtraction
    Vector3D operator+(const Vector3D& rhs) const;
    Vector3D operator+=(const Vector3D& rhs);
    Vector3D operator-(const Vector3D& rhs) const;
    Vector3D operator-=(const Vector3D& rhs);

    // Scalar operations
    Vector3D operator+(double rhs) const;
    Vector3D operator+=(double rhs);
    Vector3D operator-(double rhs) const;
    Vector3D operator-=(double rhs);
    Vector3D operator*(double rhs) const;
    Vector3D operator*=(double rhs);
    Vector3D operator/(double rhs) const;
    Vector3D operator/=(double rhs);

    void operator=(const Vector3D& rhs);

public: // utility functions
    /**
     * @brief Calculates the squared norm of the Vector3D
     */
    double norm2() const;

    /**
     * @brief Calculates the norm of the Vector3D
     */
    double norm() const;

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
     * @brief Calculates the squared norm of the Vector3D
     */
    void fill(double value);

public:
    static Vector3D X(double x_val = 1.0) { return Vector3D(x_val, 0.0, 0.0); }
    static Vector3D Y(double y_val = 1.0) { return Vector3D(0.0, y_val, 0.0); }
    static Vector3D Z(double z_val = 1.0) { return Vector3D(0.0, 0.0, z_val); }
};

} // namespace geometricAlgebra
