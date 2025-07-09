#include <enkas/math/vector3d.h>
#include <gtest/gtest.h>

TEST(Vector3DTests, DefaultConstructor) {
    enkas::math::Vector3D vec;
    EXPECT_DOUBLE_EQ(vec.x, 0.0);
    EXPECT_DOUBLE_EQ(vec.y, 0.0);
    EXPECT_DOUBLE_EQ(vec.z, 0.0);
}

TEST(Vector3DTests, ParameterizedConstructor) {
    enkas::math::Vector3D vec(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(vec.x, 1.0);
    EXPECT_DOUBLE_EQ(vec.y, 2.0);
    EXPECT_DOUBLE_EQ(vec.z, 3.0);
}

TEST(Vector3DTests, CopyConstructor) {
    enkas::math::Vector3D original(4.0, 5.0, 6.0);
    enkas::math::Vector3D copy(original);
    EXPECT_DOUBLE_EQ(copy.x, 4.0);
    EXPECT_DOUBLE_EQ(copy.y, 5.0);
    EXPECT_DOUBLE_EQ(copy.z, 6.0);
}

TEST(Vector3DTests, AssignmentOperator) {
    enkas::math::Vector3D vec1(7.0, 8.0, 9.0);
    enkas::math::Vector3D vec2;
    vec2 = vec1;
    EXPECT_DOUBLE_EQ(vec2.x, 7.0);
    EXPECT_DOUBLE_EQ(vec2.y, 8.0);
    EXPECT_DOUBLE_EQ(vec2.z, 9.0);
}

TEST(Vector3DTests, AdditionOperator) {
    enkas::math::Vector3D vec1(1.0, 2.0, 3.0);
    enkas::math::Vector3D vec2(4.0, 5.0, 6.0);
    enkas::math::Vector3D result = vec1 + vec2;
    EXPECT_DOUBLE_EQ(result.x, 5.0);
    EXPECT_DOUBLE_EQ(result.y, 7.0);
    EXPECT_DOUBLE_EQ(result.z, 9.0);
}

TEST(Vector3DTests, SubtractionOperator) {
    enkas::math::Vector3D vec1(7.0, 8.0, 9.0);
    enkas::math::Vector3D vec2(4.0, 5.0, 6.0);
    enkas::math::Vector3D result = vec1 - vec2;
    EXPECT_DOUBLE_EQ(result.x, 3.0);
    EXPECT_DOUBLE_EQ(result.y, 3.0);
    EXPECT_DOUBLE_EQ(result.z, 3.0);
}

TEST(Vector3DTests, MultiplicationOperator) {
    enkas::math::Vector3D vec(2.0, 3.0, 4.0);
    double scalar = 2.0;
    enkas::math::Vector3D result = vec * scalar;
    EXPECT_DOUBLE_EQ(result.x, 4.0);
    EXPECT_DOUBLE_EQ(result.y, 6.0);
    EXPECT_DOUBLE_EQ(result.z, 8.0);
}

TEST(Vector3DTests, DivisionOperator) {
    enkas::math::Vector3D vec(8.0, 12.0, 16.0);
    double scalar = 4.0;
    enkas::math::Vector3D result = vec / scalar;
    EXPECT_DOUBLE_EQ(result.x, 2.0);
    EXPECT_DOUBLE_EQ(result.y, 3.0);
    EXPECT_DOUBLE_EQ(result.z, 4.0);
}

TEST(Vector3DTests, Norm) {
    enkas::math::Vector3D vec(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(vec.norm(), 5.0);
}

TEST(Vector3DTests, NormSquared) {
    enkas::math::Vector3D vec(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(vec.norm2(), 25.0);
}

TEST(Vector3DTests, DotProduct) {
    enkas::math::Vector3D vec1(1.0, 2.0, 3.0);
    enkas::math::Vector3D vec2(4.0, 5.0, 6.0);
    EXPECT_DOUBLE_EQ(enkas::math::dotProduct(vec1, vec2), 32.0);
}

TEST(Vector3DTests, Normalize) {
    enkas::math::Vector3D vec(3.0, 4.0, 0.0);
    vec.set_norm(1.0);
    EXPECT_DOUBLE_EQ(vec.x, 0.6);
    EXPECT_DOUBLE_EQ(vec.y, 0.8);
    EXPECT_DOUBLE_EQ(vec.z, 0.0);
}

TEST(Vector3DTests, NormalizeZeroVector) {
    enkas::math::Vector3D vec(0.0, 0.0, 0.0);
    vec.set_norm(1.0);
    EXPECT_DOUBLE_EQ(vec.x, 0.0);
    EXPECT_DOUBLE_EQ(vec.y, 0.0);
    EXPECT_DOUBLE_EQ(vec.z, 0.0);
}

TEST(Vector3DTests, Fill) {
    enkas::math::Vector3D vec;
    vec.fill(5.0);
    EXPECT_DOUBLE_EQ(vec.x, 5.0);
    EXPECT_DOUBLE_EQ(vec.y, 5.0);
    EXPECT_DOUBLE_EQ(vec.z, 5.0);
}

TEST(Vector3DTests, CreateUnitVectors) {
    enkas::math::Vector3D x_unit = enkas::math::Vector3D::X();
    EXPECT_DOUBLE_EQ(x_unit.x, 1.0);
    EXPECT_DOUBLE_EQ(x_unit.y, 0.0);
    EXPECT_DOUBLE_EQ(x_unit.z, 0.0);

    enkas::math::Vector3D y_unit = enkas::math::Vector3D::Y();
    EXPECT_DOUBLE_EQ(y_unit.x, 0.0);
    EXPECT_DOUBLE_EQ(y_unit.y, 1.0);
    EXPECT_DOUBLE_EQ(y_unit.z, 0.0);

    enkas::math::Vector3D z_unit = enkas::math::Vector3D::Z();
    EXPECT_DOUBLE_EQ(z_unit.x, 0.0);
    EXPECT_DOUBLE_EQ(z_unit.y, 0.0);
    EXPECT_DOUBLE_EQ(z_unit.z, 1.0);
}