#pragma once

#include "core/snapshot.h"

class ISimulationWindowView {
public:
    virtual ~ISimulationWindowView() = default;

    /**
     * @brief Initializes the view for live mode.
     * In live mode, the particle rendering of the simulated system and its diagnostics charts are
     * updated in real-time.
     */
    virtual void initLiveMode() = 0;

    /**
     * @brief Updates the system rendering with the provided snapshot and simulation duration.
     * @param system_snapshot The snapshot of the system to render.
     * @param simulation_duration The duration of the simulation for which the snapshot is valid.
     */
    virtual void updateSystemRendering(SystemSnapshotPtr system_snapshot,
                                       double simulation_duration) = 0;

    /**
     * @brief Updates the charts with the latest diagnostics data.
     * @param diagnostics_snapshot The snapshot of the diagnostics data to render.
     */
    virtual void updateCharts(DiagnosticsSnapshotPtr diagnostics_snapshot) = 0;

    /**
     * @brief Updates the frames per second (FPS) display.
     * @param fps The current frames per second to display.
     */
    virtual void updateFPS(int fps) = 0;
};
