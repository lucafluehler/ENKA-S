#pragma once

#include "core/dataflow/debug_info.h"
#include "views/live_simulation_window/i_live_simulation_window_view.h"
#include "views/simulation_window/simulation_window.h"

class LiveSimulationWindow : public SimulationWindow, public ILiveSimulationWindowView {
    Q_OBJECT

public:
    explicit LiveSimulationWindow(std::shared_ptr<LiveDebugInfo> debug_info,
                                  QWidget *parent = nullptr);
    ~LiveSimulationWindow() override = default;

    void updateDiagnostics(DiagnosticsSnapshotPtr diagnostics_snapshot) override;
    void updateDebugInfo(int sps) override;

private slots:
    void toggleDebugInfo();

private:
    std::shared_ptr<LiveDebugInfo> debug_info_ = nullptr;
};
