#pragma once

#include <enkas/data/system.h>

#include <atomic>

namespace enkas::simulation {

class Simulator {
   public:
    virtual ~Simulator() = default;

    /**
     * @brief Initializes the simulator with a set of particles.
     * @param initial_system The initial state of the system to simulate.
     */
    virtual void setSystem(const data::System& initial_system) = 0;

    /**
     * @brief Advances the simulation by a single time step.
     */
    virtual void step() = 0;

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

    /**
     * @brief Returns the current state of the system.
     * @return The current state of the system.
     */
    [[nodiscard]] virtual data::System getSystem() const = 0;

   protected:
    std::atomic_bool stop_requested_{false};
};

}  // namespace enkas::simulation
