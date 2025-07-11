#pragma once

#include <enkas/logging/logger.h>
#include <enkas/logging/sinks.h>
#include <enkas/physics/helpers.h>
#include <enkas/simulation/simulator.h>
#include <gtest/gtest.h>

#include <memory>

template <typename T_Simulator, typename T_Settings>
struct SimulatorTestConfig {
    using SimulatorType = T_Simulator;
    using SettingsType = T_Settings;
};

template <typename T_Config>
class SimulatorComplianceTest : public ::testing::Test {
protected:
    using SimulatorType = typename T_Config::SimulatorType;
    using SettingsType = typename T_Config::SettingsType;

    std::unique_ptr<enkas::simulation::Simulator> simulator;
    SettingsType settings;

    static SettingsType CreateDefaultSettings();

    void SetUp() override {
        auto console_sink = std::make_shared<enkas::logging::ConsoleSink>();
        enkas::logging::getLogger().configure(enkas::logging::LogLevel::DEBUG, console_sink);

        settings = CreateDefaultSettings();
        simulator = std::make_unique<SimulatorType>(settings);
    }
};

TYPED_TEST_SUITE_P(SimulatorComplianceTest);

TYPED_TEST_P(SimulatorComplianceTest, HandlesEmptySystem) {
    enkas::data::System empty_system;

    ASSERT_NO_THROW(this->simulator->setSystem(empty_system));
    ASSERT_NO_THROW(this->simulator->step());

    EXPECT_EQ(this->simulator->getSystemTime(), 0.0);
    EXPECT_EQ(this->simulator->getSystem().count(), 0);
}

TYPED_TEST_P(SimulatorComplianceTest, SingleParticleMovesCorrectly) {
    const enkas::math::Vector3D initial_position = {1.0, 2.0, 3.0};
    const enkas::math::Vector3D initial_velocity = {10.0, 20.0, 30.0};

    enkas::data::System system;
    system.positions.push_back(initial_position);
    system.velocities.push_back(initial_velocity);
    system.masses.push_back(1.0);

    this->simulator->setSystem(system);

    // The simulator should have scaled the initial system to Hénon units. To check the
    // correctness of the simulation, we can compare the final position with the expected
    // position after one time step.
    const enkas::data::System scaled_initial_system = this->simulator->getSystem();
    const enkas::math::Vector3D scaled_initial_position = scaled_initial_system.positions[0];
    const enkas::math::Vector3D scaled_initial_velocity = scaled_initial_system.velocities[0];

    this->simulator->step();

    const enkas::data::System final_system = this->simulator->getSystem();

    EXPECT_EQ(final_system.velocities[0], scaled_initial_velocity);

    const double dt = this->simulator->getSystemTime();
    const enkas::math::Vector3D expected_position =
        scaled_initial_position + scaled_initial_velocity * dt;

    const double tolerance = 1e-9;
    EXPECT_NEAR(final_system.positions[0].x, expected_position.x, tolerance);
    EXPECT_NEAR(final_system.positions[0].y, expected_position.y, tolerance);
    EXPECT_NEAR(final_system.positions[0].z, expected_position.z, tolerance);
}

TYPED_TEST_P(SimulatorComplianceTest, EnergyIsApproximatelyConserved) {
    enkas::data::System system;
    system.positions.push_back({-1.0, 0.0, 0.0});
    system.velocities.push_back({0.0, 0.5, 0.0});
    system.masses.push_back(1.0);
    system.positions.push_back({1.0, 0.0, 0.0});
    system.velocities.push_back({0.0, -0.5, 0.0});
    system.masses.push_back(1.0);

    this->simulator->setSystem(system);

    const double softening_sqr =
        this->settings.softening_parameter * this->settings.softening_parameter;
    const double e_kin_initial = enkas::physics::getKineticEnergy(this->simulator->getSystem());
    const double e_pot_initial =
        enkas::physics::getPotentialEnergy(this->simulator->getSystem(), softening_sqr);
    const double total_energy_initial = e_kin_initial + e_pot_initial;

    for (int i = 0; i < 100; ++i) {
        this->simulator->step();
    }

    const double e_kin_final = enkas::physics::getKineticEnergy(this->simulator->getSystem());
    const double e_pot_final =
        enkas::physics::getPotentialEnergy(this->simulator->getSystem(), softening_sqr);
    const double total_energy_final = e_kin_final + e_pot_final;

    // The tolerance should be relative to the initial energy. A 5% drift is acceptable for
    // low-order integrators over this many steps. Higher-order integrators would pass a much
    // smaller tolerance. This test checks for stability, not perfect conservation.
    const double energy_tolerance = std::abs(total_energy_initial * 0.05);
    EXPECT_NEAR(total_energy_final, total_energy_initial, energy_tolerance);
}

REGISTER_TYPED_TEST_SUITE_P(SimulatorComplianceTest,
                            HandlesEmptySystem,
                            SingleParticleMovesCorrectly,
                            EnergyIsApproximatelyConserved);
