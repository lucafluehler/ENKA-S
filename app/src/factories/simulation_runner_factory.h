#pragma once

#include "factories/i_simulation_runner_factory.h"
#include "managers/simulation_runner.h"

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
