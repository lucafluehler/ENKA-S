#pragma once

#include "managers/simulation_player.h"
#include "services/simulation_player_factory/i_simulation_player_factory.h"

/**
 * @brief Concrete factory that constructs a SimulationPlayer with all its
 *        real dependencies.
 */
class SimulationPlayerFactory : public ISimulationPlayerFactory {
public:
    std::unique_ptr<ISimulationPlayer> create() override {
        return std::make_unique<SimulationPlayer>();
    }
};
