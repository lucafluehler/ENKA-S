#pragma once

#include "core/snapshot.h"

class ISimulationWindowView {
public:
    virtual ~ISimulationWindowView() = default;

    /**
     * @brief Initializes the view for live mode.
     * In live mode, the particle rendering of the simulated system and its diagnostics charts are
     * updated in real-time.
     * @param simulation_duration The total duration of the simulation, used for time navigation.
     */
    virtual void initLiveMode(double simulation_duration) = 0;

    /**
     * @brief Initializes the view for file mode.
     * In file mode, the user can navigate through a recorded simulation, allowing for step-by-step
     * playback and analysis.
     */
    virtual void initFileMode() = 0;

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
     * @brief Updates the frames per second (FPS) display.
     * @param fps The current frames per second to display.
     */
    virtual void updateFPS(int fps) = 0;
};
