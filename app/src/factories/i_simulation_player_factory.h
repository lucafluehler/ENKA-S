#pragma once

#include <memory>

class SimulationPlayer;

/**
 * @brief An interface for a factory that creates SimulationPlayer instances.
 */
class ISimulationPlayerFactory {
public:
    virtual ~ISimulationPlayerFactory() = default;

    /**
     * @brief Creates a new SimulationPlayer instance.
     * @return A unique_ptr to the created SimulationPlayer.
     */
    virtual std::unique_ptr<SimulationPlayer> create() = 0;
};
