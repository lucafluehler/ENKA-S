#include <enkas/generation/generators/stream_generator.h>
#include <enkas/logging/logger.h>
#include <enkas/logging/sinks.h>
#include <enkas/physics/helpers.h>
#include <gtest/gtest.h>

class StreamGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto console_sink = std::make_shared<enkas::logging::ConsoleSink>();
        auto test_log_level = enkas::logging::LogLevel::DEBUG;
        enkas::logging::getLogger().configure(test_log_level, console_sink);
    }
};

TEST_F(StreamGeneratorTest, CreateSystem) {
    const std::string test_data =
        "px,py,pz,vx,vy,vz,mass\n"
        "1.0, 2.0, 3.0, 10.0, 20.0, 30.0, 100.0\n"
        "4.0, 5.0, 6.0, 40.0, 50.0, 60.0, 200.0\n"
        "7.0, 8.0, 9.0, 70.0, 80.0, 90.0, 300.0\n"
        "10.0, 11.0, 12.0, 100.0, 110.0, 120.0, 400.0\n"
        "13.0, 14.0, 15.0, 130.0, 140.0, 150.0, 500.0\n";

    std::stringstream stream(test_data);

    enkas::generation::StreamGenerator generator(stream);
    enkas::data::System system = generator.createSystem();

    EXPECT_EQ(system.positions.size(), 5);
    EXPECT_EQ(system.velocities.size(), 5);
    EXPECT_EQ(system.masses.size(), 5);

    EXPECT_EQ(system.positions[0], enkas::math::Vector3D(1.0, 2.0, 3.0));
    EXPECT_EQ(system.velocities[0], enkas::math::Vector3D(10.0, 20.0, 30.0));
    EXPECT_DOUBLE_EQ(system.masses[0], 100.0);

    EXPECT_EQ(system.positions[4], enkas::math::Vector3D(13.0, 14.0, 15.0));
    EXPECT_EQ(system.velocities[4], enkas::math::Vector3D(130.0, 140.0, 150.0));
    EXPECT_DOUBLE_EQ(system.masses[4], 500.0);
}

TEST_F(StreamGeneratorTest, InvalidStream) {
    const std::string test_data =
        "px,py,pz,vx,vy,vz,mass\n"                // A valid header (will be skipped)
        "1.0, 2.0, 3.0\n"                         // Line with too few columns
        "\n"                                      // An empty line
        "a,b,c,d,e,f,g\n"                         // Line with non-numeric data
        "1.0, 2.0, 3.0, 4.0, 5.0, 1e500, 7.0\n";  // Line with a number out of range for a double

    std::stringstream test_stream(test_data);

    enkas::generation::StreamGenerator generator(test_stream);
    enkas::data::System system = generator.createSystem();

    EXPECT_EQ(system.positions.size(), 0);
    EXPECT_EQ(system.velocities.size(), 0);
    EXPECT_EQ(system.masses.size(), 0);
}