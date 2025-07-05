#include <cmath>

#include "rotor3d.h"

namespace geometricAlgebra {

Rotor3D::Rotor3D(double s, double b_xy, double b_xz, double b_yz)
    : s(s), b_xy(b_xy), b_xz(b_xz), b_yz(b_yz)
{
    // Spinnsch?
}

Rotor3D::Rotor3D(double angle, const Bivector3D& plane)
    : s(std::cos(angle/2.0))
{
    const double c_SIN = std::sin(angle/2.0);

    b_xy = -c_SIN*plane.xy;
    b_xz = -c_SIN*plane.xz;
    b_yz = -c_SIN*plane.yz;
}

Vector3D Rotor3D::rotate(const Vector3D& vec) const
{
    const Vector3D tmp{ s*vec.x + vec.y*b_xy + vec.z*b_xz
                      , s*vec.y - vec.x*b_xy + vec.z*b_yz
                      , s*vec.z - vec.x*b_xz - vec.y*b_yz };

    // trivector component
    const float t_xyz = vec.x*b_yz - vec.y*b_xz + vec.z*b_xy;
	
	return Vector3D( s*tmp.x + tmp.y*b_xy + tmp.z*b_xz + t_xyz*b_yz 
                   , s*tmp.y - tmp.x*b_xy - t_xyz*b_xz + tmp.z*b_yz
                   , s*tmp.z + t_xyz*b_xy - tmp.x*b_xz - tmp.y*b_yz );
}

double Rotor3D::norm2() const
{
    return s*s + b_xy*b_xy + b_xz*b_xz + b_yz*b_yz;
}

double Rotor3D::norm() const
{
    return std::sqrt(norm2());
}

Rotor3D Rotor3D::normalize()
{
    double rotor_norm = norm();

    s /= rotor_norm;
    b_xy /= rotor_norm;
    b_xz /= rotor_norm;
    b_yz /= rotor_norm;

    return *this;
}

Rotor3D Rotor3D::get_reverse() const
{
    return Rotor3D(s, -b_xy, -b_xz, -b_yz);
}

Rotor3D Rotor3D::operator*(const Rotor3D& rhs) const
{
    return Rotor3D( s*rhs.s - b_xy*rhs.b_xy - b_xz*rhs.b_xz - b_yz*rhs.b_yz
                  , b_xy*rhs.s + s*rhs.b_xy + b_yz*rhs.b_xz - b_xz*rhs.b_yz
                  , b_xz*rhs.s + s*rhs.b_xz - b_yz*rhs.b_xy + b_xy*rhs.b_yz
                  , b_yz*rhs.s + s*rhs.b_yz + b_xz*rhs.b_xy - b_xy*rhs.b_xz );
}

} // namespace geometricAlgebra
