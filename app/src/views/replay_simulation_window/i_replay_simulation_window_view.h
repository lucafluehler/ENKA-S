#pragma once

#include "core/dataflow/snapshot.h"
#include "views/simulation_window/i_simulation_window_view.h"

class IReplaySimulationWindowView : virtual public ISimulationWindowView {
public:
    virtual ~IReplaySimulationWindowView() = default;

    /**
     * @brief Populate the charts with a batch of diagnostics data.
     * @param series The diagnostics data series to fill the charts with.
     */
    virtual void fillCharts(const DiagnosticsSeries& series) = 0;
};