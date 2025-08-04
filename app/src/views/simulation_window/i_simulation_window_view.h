#pragma once

#include "core/dataflow/snapshot.h"

class ISimulationWindowView {
public:
    virtual ~ISimulationWindowView() = default;
    /**
     * @brief Updates the system rendering with the provided snapshot.
     * @param system_snapshot The snapshot of the system to render.
     */
    virtual void updateSystemRendering(SystemSnapshotPtr system_snapshot,
                                       double simulation_duration) = 0;

    /**
     * @brief Updates the debug information displayed in the UI.
     * @param fps The current frames per second to display.
     */
    virtual void updateFPS(int fps) = 0;

    /**
     * @brief Gets the target frames per second for the simulation.
     * @return The target FPS.
     */
    virtual int getTargetFPS() const = 0;
};
