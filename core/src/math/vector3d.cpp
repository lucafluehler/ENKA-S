#include <enkas/math/vector3d.h>

namespace enkas::math {

Vector3D::Vector3D(double p_x, double p_y, double p_z)
    : x(p_x), y(p_y), z(p_z)
{
    // so empty out here
}


// --- Compound Assignment Operators ---

Vector3D& Vector3D::operator+=(const Vector3D& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vector3D& Vector3D::operator*=(double rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

Vector3D& Vector3D::operator/=(double rhs)
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
}


// --- Utility Functions ---

double Vector3D::norm2() const
{
    return x*x + y*y + z*z;
}

double Vector3D::norm() const
{
    return std::sqrt(norm2());
}

Vector3D& Vector3D::set_norm(double new_norm)
{
    const double current_norm = norm();
    if (current_norm != 0.0) {
        *this *= (new_norm / current_norm);
    }
    return *this;
}

void Vector3D::fill(double value) {
    x = y = z = value;
}

} // namespace enkas::math
