#pragma once

#include <gmock/gmock.h>

#include "managers/i_simulation_player.h"
#include "services/simulation_player_factory/i_simulation_player_factory.h"

class MockSimulationPlayerFactory : public ISimulationPlayerFactory {
public:
    MOCK_METHOD(std::unique_ptr<ISimulationPlayer>, create, (), (override));
};
