#pragma once

#include <atomic>

#include <enkas/data/initial_system.h>
#include <enkas/data/render_data.h>
#include <enkas/data/diagnostics_data.h>
#include <enkas/data/analytics_data.h>

namespace enkas::simulation {

class Simulator
{
public:
    virtual ~Simulator() = default;

    /**
     * @brief Initializes the simulator with a set of particles.
     * @param initial_system The initial state of the system to simulate.
     */
    virtual void initializeSystem(const data::InitialSystem& initial_system) = 0;

    /**
     * @brief Advances the simulation by one or more internal time steps.
     */
    virtual void evolveSystem() = 0;

    /**
     * @brief Signals the simulator to stop its work at the next safe opportunity.
     */
    void requestStop() { stop_requested.store(true); }

    [[nodiscard]] virtual double getGlobalTime() const = 0;
    [[nodiscard]] virtual data::RenderData getRenderData() const = 0;
    [[nodiscard]] virtual data::DiagnosticsData getDiagnosticsData() const = 0;
    [[nodiscard]] virtual data::AnalyticsData getAnalyticsData() const = 0;

protected:
    std::atomic_bool stop_requested{false};
};

} // namespace enkas::simulation