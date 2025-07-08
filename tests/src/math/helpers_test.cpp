#include <gtest/gtest.h>
#include <enkas/math/helpers.h>

TEST(RandOnSphereTests, NormIsCorrect) {
    const unsigned int seed = 12345;
    std::mt19937 generator(seed);
    const double expected_norm = 50.0;
    const double tolerance = 1e-9;

    const auto vec = enkas::math::getRandOnSphere(generator, expected_norm);
    const double actual_norm = vec.norm();

    EXPECT_NEAR(actual_norm, expected_norm, tolerance);
}