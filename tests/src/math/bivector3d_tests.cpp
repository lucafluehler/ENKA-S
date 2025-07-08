#include <gtest/gtest.h>
#include <enkas/math/vector3d.h>
#include <enkas/math/bivector3d.h>

TEST(Bivector3DTests, DefaultConstructor) {
    enkas::math::Bivector3D bivector;
    EXPECT_DOUBLE_EQ(bivector.xy, 0.0);
    EXPECT_DOUBLE_EQ(bivector.xz, 0.0);
    EXPECT_DOUBLE_EQ(bivector.yz, 0.0);
}

TEST(Bivector3DTests, ParameterizedConstructor) {
    enkas::math::Bivector3D bivector(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(bivector.xy, 1.0);
    EXPECT_DOUBLE_EQ(bivector.xz, 2.0);
    EXPECT_DOUBLE_EQ(bivector.yz, 3.0);
}

TEST(Bivector3DTests, CopyConstructor) {
    enkas::math::Bivector3D original(4.0, 5.0, 6.0);
    enkas::math::Bivector3D copy(original);
    EXPECT_DOUBLE_EQ(copy.xy, 4.0);
    EXPECT_DOUBLE_EQ(copy.xz, 5.0);
    EXPECT_DOUBLE_EQ(copy.yz, 6.0);
}

TEST(Bivector3DTests, AssignmentOperator) {
    enkas::math::Bivector3D bivector1(7.0, 8.0, 9.0);
    enkas::math::Bivector3D bivector2;
    bivector2 = bivector1;
    EXPECT_DOUBLE_EQ(bivector2.xy, 7.0);
    EXPECT_DOUBLE_EQ(bivector2.xz, 8.0);
    EXPECT_DOUBLE_EQ(bivector2.yz, 9.0);
}

TEST(Bivector3DTests, AdditionOperator) {
    enkas::math::Bivector3D bivector1(1.0, 2.0, 3.0);
    enkas::math::Bivector3D bivector2(4.0, 5.0, 6.0);
    enkas::math::Bivector3D result = bivector1 + bivector2;
    EXPECT_DOUBLE_EQ(result.xy, 5.0);
    EXPECT_DOUBLE_EQ(result.xz, 7.0);
    EXPECT_DOUBLE_EQ(result.yz, 9.0);
}

TEST(Bivector3DTests, SubtractionOperator) {
    enkas::math::Bivector3D bivector1(7.0, 8.0, 9.0);
    enkas::math::Bivector3D bivector2(4.0, 5.0, 6.0);
    enkas::math::Bivector3D result = bivector1 - bivector2;
    EXPECT_DOUBLE_EQ(result.xy, 3.0);
    EXPECT_DOUBLE_EQ(result.xz, 3.0);
    EXPECT_DOUBLE_EQ(result.yz, 3.0);
}

TEST(Bivector3DTests, MultiplicationOperator) {
    enkas::math::Bivector3D bivector(2.0, 3.0, 4.0);
    double scalar = 2.0;
    enkas::math::Bivector3D result = bivector * scalar;
    EXPECT_DOUBLE_EQ(result.xy, 4.0);
    EXPECT_DOUBLE_EQ(result.xz, 6.0);
    EXPECT_DOUBLE_EQ(result.yz, 8.0);
}

TEST(Bivector3DTests, DivisionOperator) {
    enkas::math::Bivector3D bivector(8.0, 12.0, 16.0);
    double scalar = 2.0;
    enkas::math::Bivector3D result = bivector / scalar;
    EXPECT_DOUBLE_EQ(result.xy, 4.0);
    EXPECT_DOUBLE_EQ(result.xz, 6.0);
    EXPECT_DOUBLE_EQ(result.yz, 8.0);
}

TEST(Bivector3DTests, Norm) {
    enkas::math::Bivector3D bivector(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(bivector.norm(), 5.0);
}

TEST(Bivector3DTests, NormSquared) {
    enkas::math::Bivector3D bivector(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(bivector.norm2(), 25.0);
}

TEST(Bivector3DTests, GetPerpendicular) {
    enkas::math::Bivector3D bivector(1.0, 2.0, 3.0);
    enkas::math::Vector3D perpendicular = bivector.getPerpendicular();
    EXPECT_DOUBLE_EQ(perpendicular.x, 3.0);
    EXPECT_DOUBLE_EQ(perpendicular.y, -2.0);
    EXPECT_DOUBLE_EQ(perpendicular.z, 1.0);
}

TEST(Bivector3DTests, CreateXY) {
    enkas::math::Bivector3D bivector = enkas::math::Bivector3D::XY(2.0);
    EXPECT_DOUBLE_EQ(bivector.xy, 2.0);
    EXPECT_DOUBLE_EQ(bivector.xz, 0.0);
    EXPECT_DOUBLE_EQ(bivector.yz, 0.0);
}

TEST(Bivector3DTests, CreateXZ) {
    enkas::math::Bivector3D bivector = enkas::math::Bivector3D::XZ(3.0);
    EXPECT_DOUBLE_EQ(bivector.xy, 0.0);
    EXPECT_DOUBLE_EQ(bivector.xz, 3.0);
    EXPECT_DOUBLE_EQ(bivector.yz, 0.0);
}

TEST(Bivector3DTests, CreateYZ) {
    enkas::math::Bivector3D bivector = enkas::math::Bivector3D::YZ(4.0);
    EXPECT_DOUBLE_EQ(bivector.xy, 0.0);
    EXPECT_DOUBLE_EQ(bivector.xz, 0.0);
    EXPECT_DOUBLE_EQ(bivector.yz, 4.0);
}

TEST(Bivector3DTests, WedgeProduct) {
    enkas::math::Vector3D vector1(1.0, 2.0, 3.0);
    enkas::math::Vector3D vector2(4.0, 5.0, 6.0);
    enkas::math::Bivector3D bivector = enkas::math::wedge(vector1, vector2);
    EXPECT_DOUBLE_EQ(bivector.xy, -3.0);
    EXPECT_DOUBLE_EQ(bivector.xz, -6.0);
    EXPECT_DOUBLE_EQ(bivector.yz, -3.0);
}
