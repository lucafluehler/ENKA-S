#pragma once

#include "managers/simulation_runner.h"
#include "services/simulation_runner_factory/i_simulation_runner_factory.h"

/**
 * @brief Concrete factory that constructs a SimulationRunner with all its
 *        real dependencies.
 */
class SimulationRunnerFactory : public ISimulationRunnerFactory {
public:
    std::unique_ptr<ISimulationRunner> create(const Settings& settings) override {
        return std::make_unique<SimulationRunner>(settings);
    }
};
