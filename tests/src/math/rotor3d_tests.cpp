#include <gtest/gtest.h>
#include <enkas/math/vector3d.h>
#include <enkas/math/bivector3d.h>
#include <enkas/math/rotor3d.h>

TEST(Rotor3DTests, DefaultConstructor) {
    enkas::math::Rotor3D rotor;
    EXPECT_DOUBLE_EQ(rotor.s, 0.0);
    EXPECT_DOUBLE_EQ(rotor.b_xy, 0.0);
    EXPECT_DOUBLE_EQ(rotor.b_xz, 0.0);
    EXPECT_DOUBLE_EQ(rotor.b_yz, 1.0);
}

TEST(Rotor3DTests, ParameterizedConstructor) {
    enkas::math::Rotor3D rotor(0.5, 1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(rotor.s, 0.5);
    EXPECT_DOUBLE_EQ(rotor.b_xy, 1.0);
    EXPECT_DOUBLE_EQ(rotor.b_xz, 2.0);
    EXPECT_DOUBLE_EQ(rotor.b_yz, 3.0);
}

TEST(Rotor3DTests, BivectorConstructor) {
    enkas::math::Bivector3D plane(1.0, 2.0, 3.0);
    enkas::math::Rotor3D rotor(0.5, plane);
    EXPECT_DOUBLE_EQ(rotor.s, 0.5);
    EXPECT_DOUBLE_EQ(rotor.b_xy, 1.0);
    EXPECT_DOUBLE_EQ(rotor.b_xz, 2.0);
    EXPECT_DOUBLE_EQ(rotor.b_yz, 3.0);
}

TEST(Rotor3DTests, CopyConstructor) {
    enkas::math::Rotor3D original(0.5, 1.0, 2.0, 3.0);
    enkas::math::Rotor3D copy(original);
    EXPECT_DOUBLE_EQ(copy.s, 0.5);
    EXPECT_DOUBLE_EQ(copy.b_xy, 1.0);
    EXPECT_DOUBLE_EQ(copy.b_xz, 2.0);
    EXPECT_DOUBLE_EQ(copy.b_yz, 3.0);
}

TEST(Rotor3DTests, AssignmentOperator) {
    enkas::math::Rotor3D rotor1(0.5, 1.0, 2.0, 3.0);
    enkas::math::Rotor3D rotor2;
    rotor2 = rotor1;
    EXPECT_DOUBLE_EQ(rotor2.s, 0.5);
    EXPECT_DOUBLE_EQ(rotor2.b_xy, 1.0);
    EXPECT_DOUBLE_EQ(rotor2.b_xz, 2.0);
    EXPECT_DOUBLE_EQ(rotor2.b_yz, 3.0);
}

TEST(Rotor3DTests, MultiplicationOperator) {
    enkas::math::Rotor3D rotor1(0.5, 1.0, 2.0, 3.0);
    enkas::math::Rotor3D rotor2(0.5, 4.0, 5.0, 6.0);
    enkas::math::Rotor3D result = rotor1 * rotor2;
    EXPECT_DOUBLE_EQ(result.s, 0.25);
    EXPECT_DOUBLE_EQ(result.b_xy, 6.0);
    EXPECT_DOUBLE_EQ(result.b_xz, 13.0);
    EXPECT_DOUBLE_EQ(result.b_yz, 18.0);
}

TEST(Rotor3DTests, Norm) {
    enkas::math::Rotor3D rotor(0.5, 1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(rotor.norm(), 1.0);
}

TEST(Rotor3DTests, NormSquared) {
    enkas::math::Rotor3D rotor(0.5, 1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(rotor.norm2(), 1.0);
}

TEST(Rotor3DTests, Normalize) {
    enkas::math::Rotor3D rotor(0.5, 1.0, 2.0, 3.0);
    rotor.normalize();
    EXPECT_DOUBLE_EQ(rotor.norm(), 1.0);
}

TEST(Rotor3DTests, Rotate) {
    enkas::math::Rotor3D rotor(0.5, 1.0, 2.0, 3.0);
    enkas::math::Vector3D vec(1.0, 2.0, 3.0);
    enkas::math::Vector3D rotated_vec = rotor.rotate(vec);
    EXPECT_DOUBLE_EQ(rotated_vec.x, 1.0);
    EXPECT_DOUBLE_EQ(rotated_vec.y, 2.0);
    EXPECT_DOUBLE_EQ(rotated_vec.z, 3.0);
}

TEST(Rotor3DTests, GetReverse) {
    enkas::math::Rotor3D rotor(0.5, 1.0, 2.0, 3.0);
    enkas::math::Rotor3D reverse_rotor = rotor.get_reverse();
    EXPECT_DOUBLE_EQ(reverse_rotor.s, 0.5);
    EXPECT_DOUBLE_EQ(reverse_rotor.b_xy, -1.0);
    EXPECT_DOUBLE_EQ(reverse_rotor.b_xz, -2.0);
    EXPECT_DOUBLE_EQ(reverse_rotor.b_yz, -3.0);
}

TEST(Rotor3DTests, XYPlane) {
    enkas::math::Rotor3D rotor = enkas::math::Rotor3D::XY(1.0);
    EXPECT_DOUBLE_EQ(rotor.s, 1.0);
    EXPECT_DOUBLE_EQ(rotor.b_xy, 1.0);
    EXPECT_DOUBLE_EQ(rotor.b_xz, 0.0);
    EXPECT_DOUBLE_EQ(rotor.b_yz, 0.0);
}

TEST(Rotor3DTests, XZPlane) {
    enkas::math::Rotor3D rotor = enkas::math::Rotor3D::XZ(1.0);
    EXPECT_DOUBLE_EQ(rotor.s, 1.0);
    EXPECT_DOUBLE_EQ(rotor.b_xy, 0.0);
    EXPECT_DOUBLE_EQ(rotor.b_xz, 1.0);
    EXPECT_DOUBLE_EQ(rotor.b_yz, 0.0);
}

TEST(Rotor3DTests, YZPlane) {
    enkas::math::Rotor3D rotor = enkas::math::Rotor3D::YZ(1.0);
    EXPECT_DOUBLE_EQ(rotor.s, 1.0);
    EXPECT_DOUBLE_EQ(rotor.b_xy, 0.0);
    EXPECT_DOUBLE_EQ(rotor.b_xz, 0.0);
    EXPECT_DOUBLE_EQ(rotor.b_yz, 1.0);
}
