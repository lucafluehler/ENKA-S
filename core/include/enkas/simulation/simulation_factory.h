#pragma once

#include <enkas/simulation/simulation_settings.h>

#include <memory>

namespace enkas::simulation {
class Simulator;
}  // namespace enkas::simulation

namespace enkas::simulation {

class Factory {
public:
    /**
     * @brief Creates a simulator based on a settings configuration object.
     * @param config The configuration object containing the specific settings.
     * @return A unique_ptr to the created simulator, or nullptr if settings are invalid.
     */
    [[nodiscard]] static std::unique_ptr<Simulator> create(const Settings& settings);
};

}  // namespace enkas::simulation
