#pragma once

#include <memory>

#include "core/dataflow/debug_info.h"
#include "core/dataflow/snapshot.h"

class ISimulationWindowView {
public:
    virtual ~ISimulationWindowView() = default;

    /**
     * @brief Initializes the view for live mode.
     * In live mode, the particle rendering of the simulated system and its diagnostics charts are
     * updated in real-time.
     * @param debug_info Shared pointer to the debug information for live mode.
     */
    virtual void initLiveMode(std::shared_ptr<LiveDebugInfo> debug_info) = 0;

    /**
     * @brief Initializes the view for replay mode.
     * In replay mode, the particle rendering of the simulated system and its diagnostics charts are
     * updated based on recorded data.
     * @param timestamps Shared pointer to a vector of timestamps for the replay.
     * @param diagnostics_series Shared pointer to the diagnostics data for the charts.
     */
    virtual void initReplayMode(std::shared_ptr<std::vector<double>> timestamps,
                                std::shared_ptr<DiagnosticsSeries> diagnostics_series) = 0;

    /**
     * @brief Updates the system rendering with the provided snapshot.
     * @param system_snapshot The snapshot of the system to render.
     */
    virtual void updateSystemRendering(SystemSnapshotPtr system_snapshot) = 0;

    /**
     * @brief Updates the charts with the latest diagnostics data.
     * @param diagnostics_snapshot The snapshot of the diagnostics data to render.
     */
    virtual void updateCharts(DiagnosticsSnapshotPtr diagnostics_snapshot) = 0;

    /**
     * @brief Populate the charts with a batch of diagnostics data.
     * @param series The diagnostics data series to fill the charts with.
     */
    virtual void fillCharts(const DiagnosticsSeries& series) = 0;

    /**
     * @brief Updates the debug information displayed in the UI.
     * @param fps The current frames per second to display.
     * @param sps The current steps per second to display.
     */
    virtual void updateDebugInfo(int fps, int sps) = 0;

    /**
     * @brief Gets the target frames per second for the simulation.
     * @return The target FPS.
     */
    virtual int getTargetFPS() const = 0;
};
