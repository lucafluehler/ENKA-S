#ifndef GEOMETRIC_ALGEBRA_H_
#define GEOMETRIC_ALGEBRA_H_

#include "vector3d.h"
#include "bivector3d.h"
#include "rotor3d.h"

namespace ga = geometricAlgebra;

namespace geometricAlgebra {

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
inline Bivector3D wedge(const Vector3D& lhs, const Vector3D& rhs)
{
    return Bivector3D( lhs.x*rhs.y - lhs.y*rhs.x
                     , lhs.x*rhs.z - lhs.z*rhs.x
                     , lhs.y*rhs.z - lhs.z*rhs.y );
}

} // namespace geometricAlgebra

#endif // GEOMETRIC_ALGEBRA_H_
