#pragma once

#include <enkas/data/diagnostics.h>
#include <enkas/data/system.h>

#include <atomic>
#include <memory>

namespace enkas::simulation {

class Simulator {
public:
    virtual ~Simulator() = default;

    /**
     * @brief Initializes the simulator with a set of particles.
     * @param initial_system The initial state of the system to simulate.
     * @param system_buffer A shared pointer to a pre-allocated system buffer for storing the state
     * of the new system.
     */
    virtual void initialize(std::shared_ptr<data::System> initial_system,
                            std::shared_ptr<data::System> system_buffer) = 0;

    /**
     * @brief Advances the simulation by a single time step.
     * @param system_buffer An optional shared pointer to a system buffer for storing the state of
     * the new system. A copy might be made to be used as the previous state for the next step.
     * @param diagnostics_buffer An optional shared pointer to a diagnostics buffer for storing
     * diagnostics data. If provided, the simulator will fill it with the current diagnostics data.
     */
    virtual void step(std::shared_ptr<data::System> system_buffer = nullptr,
                      std::shared_ptr<data::Diagnostics> diagnostics_buffer = nullptr) = 0;

    /**
     * @brief Signals the simulator to stop its work at the next safe opportunity.
     */
    void requestStop() { stop_requested_.store(true); }

    /**
     * @brief Checks if a stop has been requested.
     * @return True if a stop has been requested, false otherwise.
     */
    [[nodiscard]] bool isStopRequested() const { return stop_requested_.load(); }

    /**
     * @brief Returns the current time of the simulation.
     * @return The current time of the simulation.
     */
    [[nodiscard]] virtual double getSystemTime() const = 0;

protected:
    std::atomic_bool stop_requested_{false};
};

}  // namespace enkas::simulation
