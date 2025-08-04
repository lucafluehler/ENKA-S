#pragma once

#include "core/dataflow/snapshot.h"
#include "views/simulation_window/i_simulation_window_view.h"

class ILiveSimulationWindowView : virtual public ISimulationWindowView {
public:
    virtual ~ILiveSimulationWindowView() = default;

    /**
     * @brief Updates the charts and the particle renderer with the latest diagnostics data.
     * @param diagnostics_snapshot The snapshot of the diagnostics data to render.
     */
    virtual void updateDiagnostics(DiagnosticsSnapshotPtr diagnostics_snapshot) = 0;

    /**
     * @brief Updates the debug information displayed in the UI.
     * @param sps The current steps per second to display.
     */
    virtual void updateDebugInfo(int sps) = 0;
};
