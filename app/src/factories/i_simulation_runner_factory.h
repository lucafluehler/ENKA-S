#pragma once

#include <memory>

#include "core/settings/settings.h"

class SimulationRunner;

/**
 * @brief An interface for a factory that creates SimulationRunner instances.
 */
class ISimulationRunnerFactory {
public:
    virtual ~ISimulationRunnerFactory() = default;

    /**
     * @brief Creates a new SimulationRunner instance.
     * @param settings The runtime settings configured by the user.
     * @return A unique_ptr to the created SimulationRunner.
     */
    virtual std::unique_ptr<SimulationRunner> create(const Settings& settings) = 0;
};
