#include <enkas/generation/generators/spiral_galaxy_generator.h>
#include <enkas/physics/helpers.h>
#include <gtest/gtest.h>

class SpiralGalaxyGeneratorTest : public ::testing::Test {
protected:
    enkas::generation::SpiralGalaxySettings settings{};

    void SetUp() override {
        settings.particle_count = 1000;
        settings.num_arms = 2;
        settings.radius = 10.0;
        settings.total_mass = 1.0e12;
        settings.twist = 0.5;
        settings.black_hole_mass = 1.0e9;
    }
};

TEST_F(SpiralGalaxyGeneratorTest, CreateSystem) {
    enkas::generation::SpiralGalaxyGenerator generator(settings, 42);
    enkas::data::System system = generator.createSystem();

    EXPECT_EQ(system.positions.size(), settings.particle_count + 1);  // +1 for the black hole
    EXPECT_EQ(system.velocities.size(), settings.particle_count + 1);
    EXPECT_EQ(system.masses.size(), settings.particle_count + 1);

    // Test position and velocity distributions
    // ...

    const double total_mass = std::accumulate(system.masses.begin(), system.masses.end(), 0.0);
    EXPECT_NEAR(total_mass, settings.total_mass, 1e6);
}

TEST_F(SpiralGalaxyGeneratorTest, SystemCentered) {
    enkas::generation::SpiralGalaxyGenerator generator(settings, 42);
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

TEST_F(SpiralGalaxyGeneratorTest, Reproducibility) {
    enkas::generation::SpiralGalaxyGenerator generator1(settings, 42);
    enkas::data::System system1 = generator1.createSystem();

    enkas::generation::SpiralGalaxyGenerator generator2(settings, 42);
    enkas::data::System system2 = generator2.createSystem();

    ASSERT_EQ(system1.positions.size(), system2.positions.size());

    for (size_t i = 0; i < system1.positions.size(); ++i) {
        EXPECT_EQ(system1.positions[i], system2.positions[i]);
        EXPECT_EQ(system1.velocities[i], system2.velocities[i]);
        EXPECT_DOUBLE_EQ(system1.masses[i], system2.masses[i]);
    }
}
