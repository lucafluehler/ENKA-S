#include <enkas/math/bivector3d.h>

namespace enkas {
namespace math {

Bivector3D::Bivector3D(double xy, double xz, double yz)
    : xy(xy), xz(xz), yz(yz)
{
    // Do not feed the bivectors
}

double Bivector3D::norm2() const
{
    return xy*xy + xz*xz + yz*yz;
}

double Bivector3D::norm() const
{
    return std::sqrt(norm2());
}

Bivector3D Bivector3D::operator+(const Bivector3D& rhs) const
{
    return Bivector3D(xy + rhs.xy, xz + rhs.xz, yz + rhs.yz);
}

Bivector3D Bivector3D::operator+=(const Bivector3D& rhs)
{
    (*this) = (*this) + rhs;
    return *this;
}

Vector3D Bivector3D::getPerpendicular() const
{
    return Vector3D(yz, -xz, xy);
}

} // namespace math
} // namespace enkas
