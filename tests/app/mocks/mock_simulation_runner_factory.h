#pragma once

#include <gmock/gmock.h>

#include "managers/i_simulation_runner.h"
#include "services/simulation_runner_factory/i_simulation_runner_factory.h"

class MockSimulationRunnerFactory : public ISimulationRunnerFactory {
public:
    MOCK_METHOD(std::unique_ptr<ISimulationRunner>, create, (const Settings&), (override));
};
