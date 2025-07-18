#pragma once

#include "core/snapshot.h"

class ISimulationWindowView {
public:
    virtual ~ISimulationWindowView() = default;

    virtual void initLiveMode() = 0;

    virtual void updateSystemRendering(SystemSnapshotPtr system_snapshot,
                                       double simulation_duration,
                                       double fps) = 0;
};
