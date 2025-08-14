#pragma once

#include <gmock/gmock.h>

#include "factories/i_simulation_runner_factory.h"
#include "managers/i_simulation_runner.h"

class MockSimulationRunnerFactory : public ISimulationRunnerFactory {
public:
    MOCK_METHOD(std::unique_ptr<ISimulationRunner>, create, (const Settings&), (override));
};
