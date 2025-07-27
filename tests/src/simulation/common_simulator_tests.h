#pragma once

#include <enkas/logging/logger.h>
#include <enkas/logging/sinks.h>
#include <enkas/physics/helpers.h>
#include <enkas/simulation/simulator.h>
#include <gtest/gtest.h>

#include <memory>

#include "enkas/data/system.h"

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
    auto empty_system = std::make_shared<enkas::data::System>();
    auto temp_buffer = std::make_shared<enkas::data::System>();

    auto system_output_buffer = std::make_shared<enkas::data::System>();
    auto diagnostics_output_buffer = std::make_shared<enkas::data::Diagnostics>();

    ASSERT_NO_THROW(this->simulator->initialize(empty_system, temp_buffer));

    ASSERT_NO_THROW(this->simulator->step(system_output_buffer, diagnostics_output_buffer));

    EXPECT_EQ(this->simulator->getSystemTime(), 0.0);
    EXPECT_EQ(system_output_buffer->count(), 0);
    EXPECT_EQ(diagnostics_output_buffer->e_kin, 0.0);
}

TYPED_TEST_P(SimulatorComplianceTest, SingleParticleMovesCorrectly) {
    const enkas::math::Vector3D initial_position = {1.0, 2.0, 3.0};
    const enkas::math::Vector3D initial_velocity = {10.0, 20.0, 30.0};

    auto system = std::make_shared<enkas::data::System>();
    system->positions.push_back(initial_position);
    system->velocities.push_back(initial_velocity);
    system->masses.push_back(1.0);

    auto temp_buffer = std::make_shared<enkas::data::System>(system->count());
    this->simulator->initialize(system, temp_buffer);

    ENKAS_LOG_DEBUG("System size: {}", system->count());
    ENKAS_LOG_DEBUG("Temp buffer size: {}", temp_buffer->count());

    const enkas::math::Vector3D scaled_initial_position = system->positions[0];
    const enkas::math::Vector3D scaled_initial_velocity = system->velocities[0];

    auto system_output_buffer = std::make_shared<enkas::data::System>(system->count());
    ENKAS_LOG_DEBUG("System output buffer size: {}", system_output_buffer->count());
    this->simulator->step(system_output_buffer);
    ENKAS_LOG_DEBUG("System output buffer size: {}", system_output_buffer->count());

    // Since a single particle has no forces, its velocity should not change.
    EXPECT_EQ(system_output_buffer->velocities[0], scaled_initial_velocity);

    const double dt = this->simulator->getSystemTime();
    const enkas::math::Vector3D expected_position =
        scaled_initial_position + scaled_initial_velocity * dt;

    const double tolerance = 1e-9;
    EXPECT_NEAR(system_output_buffer->positions[0].x, expected_position.x, tolerance);
    EXPECT_NEAR(system_output_buffer->positions[0].y, expected_position.y, tolerance);
    EXPECT_NEAR(system_output_buffer->positions[0].z, expected_position.z, tolerance);
}

TYPED_TEST_P(SimulatorComplianceTest, EnergyIsApproximatelyConserved) {
    auto system = std::make_shared<enkas::data::System>();
    system->positions.push_back({-1.0, 0.0, 0.0});
    system->velocities.push_back({0.0, 0.5, 0.0});
    system->masses.push_back(1.0);
    system->positions.push_back({1.0, 0.0, 0.0});
    system->velocities.push_back({0.0, -0.5, 0.0});
    system->masses.push_back(1.0);

    auto temp_buffer = std::make_shared<enkas::data::System>(system->count());
    this->simulator->initialize(system, temp_buffer);

    const double softening_sqr =
        this->settings.softening_parameter * this->settings.softening_parameter;

    // Calculate initial energy from the scaled system.
    const double e_kin_initial = enkas::physics::getKineticEnergy(*system);
    const double e_pot_initial = enkas::physics::getPotentialEnergy(*system, softening_sqr);
    ENKAS_LOG_DEBUG("Initial potential energy: {:.4e}", e_pot_initial);
    ENKAS_LOG_DEBUG("Initial kinetic energy: {:.4e}", e_kin_initial);
    const double total_energy_initial = e_kin_initial + e_pot_initial;
    ENKAS_LOG_DEBUG("Initial total energy: {:.4e}", total_energy_initial);

    // Run the simulation and retrieve data after 100 steps.
    for (int i = 0; i < 99; ++i) {
        this->simulator->step();
    }

    auto system_output_buffer = std::make_shared<enkas::data::System>(system->count());
    this->simulator->step(system_output_buffer);

    // Calculate final total energy from the output system.
    const double e_kin_final = enkas::physics::getKineticEnergy(*system_output_buffer);
    const double e_pot_final =
        enkas::physics::getPotentialEnergy(*system_output_buffer, softening_sqr);
    ENKAS_LOG_DEBUG("Final potential energy: {:.4e}", e_pot_final);
    ENKAS_LOG_DEBUG("Final kinetic energy: {:.4e}", e_kin_final);
    const double total_energy_final = e_kin_final + e_pot_final;
    ENKAS_LOG_DEBUG("Final total energy: {:.4e}", total_energy_final);

    const double energy_tolerance = std::abs(total_energy_initial * 0.05);
    EXPECT_NEAR(total_energy_final, total_energy_initial, energy_tolerance);
}

REGISTER_TYPED_TEST_SUITE_P(SimulatorComplianceTest,
                            HandlesEmptySystem,
                            SingleParticleMovesCorrectly,
                            EnergyIsApproximatelyConserved);
