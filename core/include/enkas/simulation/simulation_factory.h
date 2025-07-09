#pragma once

#include <memory>

// Forward declarations
namespace enkas::simulation {
class Simulator;
struct SimulationConfig;
}  // namespace enkas::simulation

namespace enkas::simulation {

class SimulationFactory {
public:
    /**
     * @brief Creates a simulator based on a settings configuration object.
     * @param config The configuration object containing the specific settings.
     * @return A shared_ptr to the created simulator, or nullptr if config is invalid.
     */
    [[nodiscard]] static std::shared_ptr<Simulator> create(const SimulationConfig& config);
};

}  // namespace enkas::simulation
