#pragma once

#include <QObject>
#include <atomic>
#include <memory>

#include "simulation_window_presenter.h"
#include "views/replay_simulation_window/i_replay_simulation_window_view.h"

class ReplaySimulationWindowPresenter : public SimulationWindowPresenter {
    Q_OBJECT
public:
    explicit ReplaySimulationWindowPresenter(
        IReplaySimulationWindowView* view,
        std::shared_ptr<std::atomic<SystemSnapshotPtr>> rendering_snapshot,
        double simulation_duration,
        QObject* parent = nullptr);
    ~ReplaySimulationWindowPresenter() override = default;

private:
    IReplaySimulationWindowView* view_;
};
