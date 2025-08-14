#pragma once

#include "factories/i_simulation_player_factory.h"
#include "managers/simulation_player.h"

/**
 * @brief Concrete factory that constructs a SimulationPlayer with all its
 *        real dependencies.
 */
class SimulationPlayerFactory : public ISimulationPlayerFactory {
public:
    std::unique_ptr<SimulationPlayer> create() override {
        return std::make_unique<SimulationPlayer>();
    }
};
