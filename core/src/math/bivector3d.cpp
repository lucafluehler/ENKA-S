#include <enkas/math/bivector3d.h>

namespace enkas::math {

Bivector3D::Bivector3D(double xy, double xz, double yz)
    : xy(xy), xz(xz), yz(yz)
{
    // Do not feed the bivectors
}


// --- Compound Assignment Operators ---

Bivector3D& Bivector3D::operator+=(const Bivector3D& rhs)
{
    xy += rhs.xy;
    xz += rhs.xz;
    yz += rhs.yz;
    return *this;
}

Bivector3D& Bivector3D::operator-=(const Bivector3D& rhs)
{
    xy -= rhs.xy;
    xz -= rhs.xz;
    yz -= rhs.yz;
    return *this;
}

Bivector3D& Bivector3D::operator*=(double scalar)
{
    xy *= scalar;
    xz *= scalar;
    yz *= scalar;
    return *this;
}

Bivector3D& Bivector3D::operator/=(double scalar)
{
    xy /= scalar;
    xz /= scalar;
    yz /= scalar;
    return *this;
}


// --- Utility Functions ---

double Bivector3D::norm2() const
{
    return xy*xy + xz*xz + yz*yz;
}

double Bivector3D::norm() const
{
    return std::sqrt(norm2());
}

Vector3D Bivector3D::getPerpendicular() const
{
    return Vector3D(yz, -xz, xy);
}

} // namespace enkas::math
