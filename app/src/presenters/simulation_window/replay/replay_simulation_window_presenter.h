#pragma once

#include <QObject>
#include <memory>

#include "core/dataflow/latest_value_slot.h"
#include "presenters/simulation_window/simulation_window_presenter.h"
#include "views/simulation_window/replay/i_replay_simulation_window_view.h"

class ReplaySimulationWindowPresenter : public SimulationWindowPresenter {
    Q_OBJECT
public:
    explicit ReplaySimulationWindowPresenter(
        IReplaySimulationWindowView* view,
        std::shared_ptr<LatestValueSlot<SystemSnapshot>> rendering_snapshot,
        double simulation_duration,
        QObject* parent = nullptr);
    ~ReplaySimulationWindowPresenter() override = default;

private:
    IReplaySimulationWindowView* view_;
};
