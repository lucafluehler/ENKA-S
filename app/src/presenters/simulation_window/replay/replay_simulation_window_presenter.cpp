#include "replay_simulation_window_presenter.h"

ReplaySimulationWindowPresenter::ReplaySimulationWindowPresenter(
    IReplaySimulationWindowView* view,
    std::shared_ptr<LatestValueSlot<SystemSnapshot>> rendering_snapshot,
    double simulation_duration,
    QObject* parent)
    : SimulationWindowPresenter(view, rendering_snapshot, simulation_duration, parent),
      view_(view) {}