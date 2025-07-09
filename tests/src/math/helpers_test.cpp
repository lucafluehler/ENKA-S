#include <enkas/math/helpers.h>
#include <gtest/gtest.h>

TEST(RandOnSphereTests, NormIsCorrect) {
    const unsigned int seed = 12345;
    std::mt19937 generator(seed);
    const double expected_norm = 50.0;
    const double tolerance = 1e-9;

    const auto vec = enkas::math::getRandOnSphere(generator, expected_norm);
    const double actual_norm = vec.norm();

    EXPECT_NEAR(actual_norm, expected_norm, tolerance);
}

TEST(RandOnSphereTests, DistributionIsUniform) {
    const unsigned int seed = 12345;
    std::mt19937 generator(seed);
    const unsigned int sample_count = 100000;

    enkas::math::Vector3D mean_vector;

    for (unsigned int i = 0; i < sample_count; ++i) {
        auto vec = enkas::math::getRandOnSphere(generator);
        mean_vector += vec * (1.0 / sample_count);
    }

    // The mean vector magnitude should be close to zero for a
    // uniform distribution on the sphere
    const double tolerance = 1e-2;
    EXPECT_NEAR(mean_vector.norm(), 0.0, tolerance);
}
