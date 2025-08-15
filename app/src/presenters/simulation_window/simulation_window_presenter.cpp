#include "simulation_window_presenter.h"

#include <enkas/logging/logger.h>

#include <QObject>
#include <QThread>
#include <QTimer>
#include <atomic>
#include <chrono>
#include <memory>

#include "core/dataflow/snapshot.h"
#include "views/simulation_window/i_simulation_window_view.h"

SimulationWindowPresenter::SimulationWindowPresenter(
    ISimulationWindowView* view,
    std::shared_ptr<std::atomic<SystemSnapshotPtr>> rendering_snapshot,
    double simulation_duration,
    QObject* parent)
    : QObject(parent),
      view_(view),
      rendering_snapshot_(rendering_snapshot),
      render_timer_(new QTimer(this)),
      simulation_duration_(simulation_duration),
      last_fps_update_time_(std::chrono::steady_clock::now()) {
    connect(render_timer_, &QTimer::timeout, this, &SimulationWindowPresenter::updateRendering);
    render_timer_->start(1000 / view_->getTargetFPS());
}

void SimulationWindowPresenter::updateRendering() {
    if (!rendering_snapshot_) {
        ENKAS_LOG_ERROR("Rendering snapshot is not set. Cannot update rendering.");
        return;
    }

    // If the rendering is faster than the simulation, we do not want to drop frames, but instead
    // pass the nullptr to the view, which must handle it properly.
    auto system_snapshot = rendering_snapshot_->exchange(nullptr, std::memory_order_acq_rel);

    // Update the view with the system snapshot
    view_->updateSystemRendering(system_snapshot, simulation_duration_);
    frame_count_++;

    // Check if it's time to update the debug info
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed_time = now - last_fps_update_time_;
    const auto fps_display_update_interval = std::chrono::milliseconds(300);
    if (elapsed_time < fps_display_update_interval) return;

    // Calculate FPS
    const double elapsed_seconds = std::chrono::duration<double>(elapsed_time).count();
    const int fps = static_cast<int>(frame_count_ / elapsed_seconds);

    view_->updateFPS(fps);
    frame_count_ = 0;  // Reset frame count for the next interval
    last_fps_update_time_ = now;
}
