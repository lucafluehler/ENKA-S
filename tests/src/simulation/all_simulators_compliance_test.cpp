#include <enkas/simulation/simulators/barneshutleapfrog_simulator.h>
#include <enkas/simulation/simulators/euler_simulator.h>
#include <enkas/simulation/simulators/hermite_simulator.h>
#include <enkas/simulation/simulators/hits_simulator.h>
#include <enkas/simulation/simulators/leapfrog_simulator.h>

#include "common_simulator_tests.h"

template <>
enkas::simulation::EulerSettings SimulatorComplianceTest<
    SimulatorTestConfig<enkas::simulation::EulerSimulator,
                        enkas::simulation::EulerSettings>>::CreateDefaultSettings() {
    enkas::simulation::EulerSettings settings;
    settings.time_step = 0.01;
    settings.softening_parameter = 0.01;
    return settings;
}

template <>
enkas::simulation::LeapfrogSettings SimulatorComplianceTest<
    SimulatorTestConfig<enkas::simulation::LeapfrogSimulator,
                        enkas::simulation::LeapfrogSettings>>::CreateDefaultSettings() {
    enkas::simulation::LeapfrogSettings settings;
    settings.time_step = 0.01;
    settings.softening_parameter = 0.01;
    return settings;
}

template <>
enkas::simulation::HermiteSettings SimulatorComplianceTest<
    SimulatorTestConfig<enkas::simulation::HermiteSimulator,
                        enkas::simulation::HermiteSettings>>::CreateDefaultSettings() {
    enkas::simulation::HermiteSettings settings;
    settings.time_step = 0.01;
    settings.softening_parameter = 0.01;
    return settings;
}

template <>
enkas::simulation::HitsSettings SimulatorComplianceTest<
    SimulatorTestConfig<enkas::simulation::HitsSimulator,
                        enkas::simulation::HitsSettings>>::CreateDefaultSettings() {
    enkas::simulation::HitsSettings settings;
    settings.softening_parameter = 0.01;
    settings.time_step_parameter = 0.01;
    return settings;
}

template <>
enkas::simulation::BarnesHutLeapfrogSettings SimulatorComplianceTest<
    SimulatorTestConfig<enkas::simulation::BarnesHutLeapfrogSimulator,
                        enkas::simulation::BarnesHutLeapfrogSettings>>::CreateDefaultSettings() {
    enkas::simulation::BarnesHutLeapfrogSettings settings;
    settings.time_step = 0.01;
    settings.softening_parameter = 0.01;
    settings.theta_mac = 0.5;
    return settings;
}

using SimulatorImplementationTypes = ::testing::Types<
    SimulatorTestConfig<enkas::simulation::EulerSimulator, enkas::simulation::EulerSettings>,
    SimulatorTestConfig<enkas::simulation::LeapfrogSimulator, enkas::simulation::LeapfrogSettings>,
    SimulatorTestConfig<enkas::simulation::HermiteSimulator, enkas::simulation::HermiteSettings>,
    SimulatorTestConfig<enkas::simulation::HitsSimulator, enkas::simulation::HitsSettings>,
    SimulatorTestConfig<enkas::simulation::BarnesHutLeapfrogSimulator,
                        enkas::simulation::BarnesHutLeapfrogSettings>>;

INSTANTIATE_TYPED_TEST_SUITE_P(AllSimulators,
                               SimulatorComplianceTest,
                               SimulatorImplementationTypes);
