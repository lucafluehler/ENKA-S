#include <cmath>

#include <enkas/math/rotor3d.h>

namespace enkas::math {

Rotor3D::Rotor3D(double p_s, double p_b_xy, double p_b_xz, double p_b_yz)
    : s(p_s), b_xy(p_b_xy), b_xz(p_b_xz), b_yz(p_b_yz)
{
    // Spinnsch?
}

Rotor3D::Rotor3D(double angle_rad, const Bivector3D& plane)
{
    const double sin_half_angle = std::sin(angle_rad/2.0);
    const Bivector3D unit_plane = plane / plane.norm();

    s = std::cos(angle_rad/2.0);
    b_xy = -sin_half_angle*unit_plane.xy;
    b_xz = -sin_half_angle*unit_plane.xz;
    b_yz = -sin_half_angle*unit_plane.yz;
}


// --- Compound Assignment Operator ---

Rotor3D& Rotor3D::operator*=(const Rotor3D& rhs)
{
    // Store the old 's' value as it's needed for the other calculations
    const double temp_s    = s*rhs.s - b_xy*rhs.b_xy - b_xz*rhs.b_xz - b_yz*rhs.b_yz;
    const double temp_b_xy = s*rhs.b_xy + b_xy*rhs.s - b_xz*rhs.b_yz + b_yz*rhs.b_xz;
    const double temp_b_xz = s*rhs.b_xz + b_xz*rhs.s + b_xy*rhs.b_yz - b_yz*rhs.b_xy;
    const double temp_b_yz = s*rhs.b_yz + b_yz*rhs.s - b_xy*rhs.b_xz + b_xz*rhs.b_xy;

    s = temp_s;
    b_xy = temp_b_xy;
    b_xz = temp_b_xz;
    b_yz = temp_b_yz;

    return *this;
}


// Utility Functions

double Rotor3D::norm2() const
{
    return s*s + b_xy*b_xy + b_xz*b_xz + b_yz*b_yz;
}

double Rotor3D::norm() const
{
    return std::sqrt(norm2());
}

Rotor3D& Rotor3D::normalize()
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

Vector3D Rotor3D::rotate(const Vector3D& vec) const
{
    const Vector3D tmp{ s*vec.x + vec.y*b_xy + vec.z*b_xz
                      , s*vec.y - vec.x*b_xy + vec.z*b_yz
                      , s*vec.z - vec.x*b_xz - vec.y*b_yz };

    // Trivector component
    const float t_xyz = vec.x*b_yz - vec.y*b_xz + vec.z*b_xy;
	
	return Vector3D( s*tmp.x + tmp.y*b_xy + tmp.z*b_xz + t_xyz*b_yz 
                   , s*tmp.y - tmp.x*b_xy - t_xyz*b_xz + tmp.z*b_yz
                   , s*tmp.z + t_xyz*b_xy - tmp.x*b_xz - tmp.y*b_yz );
}

} // namespace enkas::math
