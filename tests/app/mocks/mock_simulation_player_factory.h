#pragma once

#include <gmock/gmock.h>

#include "factories/i_simulation_player_factory.h"
#include "managers/i_simulation_player.h"

class MockSimulationPlayerFactory : public ISimulationPlayerFactory {
public:
    MOCK_METHOD(std::unique_ptr<ISimulationPlayer>, create, (), (override));
};
