#include <enkas/generation/generators/uniform_cube_generator.h>
#include <enkas/logging/logger.h>
#include <enkas/logging/sinks.h>
#include <enkas/physics/helpers.h>
#include <gtest/gtest.h>

class UniformCubeGeneratorTest : public ::testing::Test {
protected:
    enkas::generation::UniformCubeSettings settings{};

    void SetUp() override {
        settings.seed = 42;
        settings.particle_count = 100;
        settings.side_length = 10.0;
        settings.total_mass = 1.0;
        settings.initial_velocity = 1.0;

        auto console_sink = std::make_shared<enkas::logging::ConsoleSink>();
        auto test_log_level = enkas::logging::LogLevel::DEBUG;
        enkas::logging::getLogger().configure(test_log_level, console_sink);
    }
};

TEST_F(UniformCubeGeneratorTest, CreateSystem) {
    enkas::generation::UniformCubeGenerator generator(settings);
    enkas::data::System system = generator.createSystem();

    EXPECT_EQ(system.positions.size(), settings.particle_count);
    EXPECT_EQ(system.velocities.size(), settings.particle_count);
    EXPECT_EQ(system.masses.size(), settings.particle_count);

    const double tolerance = 1;
    for (const auto& p : system.positions) {
        EXPECT_LE(std::abs(p.x), settings.side_length / 2.0 + tolerance);
        EXPECT_LE(std::abs(p.y), settings.side_length / 2.0 + tolerance);
        EXPECT_LE(std::abs(p.z), settings.side_length / 2.0 + tolerance);
    }

    for (const auto& v : system.velocities) {
        EXPECT_LE(v.norm(), settings.initial_velocity);
    }

    const double total_mass = std::accumulate(system.masses.begin(), system.masses.end(), 0.0);
    EXPECT_NEAR(total_mass, settings.total_mass, 1e-6);
}

TEST_F(UniformCubeGeneratorTest, SystemCentered) {
    enkas::generation::UniformCubeGenerator generator(settings);
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

TEST_F(UniformCubeGeneratorTest, Reproducibility) {
    enkas::generation::UniformCubeGenerator generator1(settings);
    enkas::data::System system1 = generator1.createSystem();

    enkas::generation::UniformCubeGenerator generator2(settings);
    enkas::data::System system2 = generator2.createSystem();

    ASSERT_EQ(system1.positions.size(), system2.positions.size());

    for (size_t i = 0; i < system1.positions.size(); ++i) {
        EXPECT_EQ(system1.positions[i], system2.positions[i]);
        EXPECT_EQ(system1.velocities[i], system2.velocities[i]);
        EXPECT_DOUBLE_EQ(system1.masses[i], system2.masses[i]);
    }
}
