#pragma once

#include "core/snapshot.h"

class ISimulationWindowView {
public:
    virtual ~ISimulationWindowView() = default;

    virtual void initLiveMode() = 0;

    virtual void updateSystemRendering(SystemSnapshotPtr system_snapshot,
                                       double simulation_duration) = 0;
    virtual void updateFPS(int fps) = 0;
};
