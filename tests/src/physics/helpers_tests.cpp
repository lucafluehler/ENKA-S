#include <enkas/physics/helpers.h>
#include <gtest/gtest.h>

TEST(PhysicsHelpersTests, GetKineticEnergy) {
    enkas::data::System system;
    system.resize(3);
    system.masses = {1.0, 2.0, 3.0};
    system.velocities = {enkas::math::Vector3D(1.0, 0.0, 0.0),
                         enkas::math::Vector3D(0.0, 2.0, 0.0),
                         enkas::math::Vector3D(0.0, 0.0, 3.0)};

    EXPECT_DOUBLE_EQ(enkas::physics::getKineticEnergy(system), 18.0);
}

TEST(PhysicsHelpersTests, GetPotentialEnergy) {
    enkas::data::System system;
    system.resize(3);
    system.masses = {1.0, 2.0, 3.0};
    system.positions = {enkas::math::Vector3D(0.0, 0.0, 0.0),
                        enkas::math::Vector3D(1.0, 0.0, 0.0),
                        enkas::math::Vector3D(0.0, 1.0, 0.0)};

    const double s = 1e-6;  // Small value to avoid singularities
    const double e_pot = enkas::physics::getPotentialEnergy(system, s);
    const double tolerance = 1e-9;
    EXPECT_NEAR(e_pot, -9.2426406871157241, tolerance);
}

// Angular momentum test
TEST(PhysicsHelpersTests, GetAngularMomentum) {
    enkas::data::System system;
    system.resize(3);
    system.masses = {1.0, 2.0, 3.0};
    system.positions = {enkas::math::Vector3D(1.0, 0.0, 0.0),
                        enkas::math::Vector3D(0.0, 1.0, 0.0),
                        enkas::math::Vector3D(0.0, 0.0, 1.0)};
    system.velocities = {enkas::math::Vector3D(0.0, 1.0, 2.0),
                         enkas::math::Vector3D(2.0, 3.0, 4.0),
                         enkas::math::Vector3D(5.0, 6.0, 7.0)};

    const double angular_momentum = enkas::physics::getAngularMomentum(system).norm();
    const double tolerance = 1e-9;
    EXPECT_NEAR(angular_momentum, 16.673332000533065, tolerance);
}

TEST(PhysicsHelpersTests, CenterSystem) {
    enkas::data::System system;
    system.resize(3);
    system.masses = {1.0, 2.0, 4.0};
    system.positions = {enkas::math::Vector3D(1.0, 0.0, 0.0),
                        enkas::math::Vector3D(2.0, 0.0, 0.0),
                        enkas::math::Vector3D(4.0, 0.0, 0.0)};
    system.velocities = {enkas::math::Vector3D(1.0, 0.0, 0.0),
                         enkas::math::Vector3D(2.0, 0.0, 0.0),
                         enkas::math::Vector3D(4.0, 0.0, 0.0)};

    enkas::physics::centerSystem(system);

    EXPECT_DOUBLE_EQ(system.positions[0].x, -2.0);
    EXPECT_DOUBLE_EQ(system.positions[1].x, -1.0);
    EXPECT_DOUBLE_EQ(system.positions[2].x, 1.0);

    EXPECT_DOUBLE_EQ(system.velocities[0].x, -2.0);
    EXPECT_DOUBLE_EQ(system.velocities[1].x, -1.0);
    EXPECT_DOUBLE_EQ(system.velocities[2].x, 1.0);
}
