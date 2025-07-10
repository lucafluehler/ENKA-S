#include <enkas/generation/generators/normal_sphere_generator.h>
#include <enkas/physics/helpers.h>
#include <gtest/gtest.h>

class NormalSphereGeneratorTest : public ::testing::Test {
protected:
    enkas::generation::NormalSphereSettings settings{};

    void SetUp() override {
        settings.particle_count = 100;
        settings.position_std_dev = 5.0;
        settings.velocity_std_dev = 1.0;
        settings.mass_mean = 1.0;
        settings.mass_std_dev = 0.1;
    }
};

TEST_F(NormalSphereGeneratorTest, CreateSystem) {
    enkas::generation::NormalSphereGenerator generator(settings, 42);
    enkas::data::System system = generator.createSystem();

    EXPECT_EQ(system.positions.size(), settings.particle_count);
    EXPECT_EQ(system.velocities.size(), settings.particle_count);
    EXPECT_EQ(system.masses.size(), settings.particle_count);

    // Test position and velocity distributions
    // ...

    const double total_mass = std::accumulate(system.masses.begin(), system.masses.end(), 0.0);
    EXPECT_NEAR(total_mass, settings.particle_count * settings.mass_mean, 1e-6);
}

TEST_F(NormalSphereGeneratorTest, SystemCentered) {
    enkas::generation::NormalSphereGenerator generator(settings, 42);
    enkas::data::System system = generator.createSystem();

    auto com = enkas::physics::getCenterOfMass(system);
    const double tolerance = 1e-9;
    EXPECT_NEAR(com.position.x, 0.0, tolerance);
    EXPECT_NEAR(com.position.y, 0.0, tolerance);
    EXPECT_NEAR(com.position.z, 0.0, tolerance);

    EXPECT_NEAR(com.velocity.x, 0.0, tolerance);
    EXPECT_NEAR(com.velocity.y, 0.0, tolerance);
    EXPECT_NEAR(com.velocity.z, 0.0, tolerance);
}

TEST_F(NormalSphereGeneratorTest, Reproducibility) {
    enkas::generation::NormalSphereGenerator generator1(settings, 42);
    enkas::data::System system1 = generator1.createSystem();

    enkas::generation::NormalSphereGenerator generator2(settings, 42);
    enkas::data::System system2 = generator2.createSystem();

    ASSERT_EQ(system1.positions.size(), system2.positions.size());

    for (size_t i = 0; i < system1.positions.size(); ++i) {
        EXPECT_EQ(system1.positions[i], system2.positions[i]);
        EXPECT_EQ(system1.velocities[i], system2.velocities[i]);
        EXPECT_DOUBLE_EQ(system1.masses[i], system2.masses[i]);
    }
}