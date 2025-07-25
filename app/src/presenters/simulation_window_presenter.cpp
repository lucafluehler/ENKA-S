#include "presenters/simulation_window_presenter.h"

#include <enkas/logging/logger.h>

#include <QObject>
#include <QTimer>
#include <atomic>
#include <chrono>
#include <memory>

#include "core/dataflow/blocking_queue.h"
#include "core/dataflow/snapshot.h"
#include "views/simulation_window/i_simulation_window_view.h"

SimulationWindowPresenter::SimulationWindowPresenter(ISimulationWindowView* view, QObject* parent)
    : QObject(parent),
      view_(view),
      mode_(Mode::Uninitialized),
      render_timer_(new QTimer(this)),
      last_debug_info_update_time_(std::chrono::steady_clock::now()) {
    connect(render_timer_, &QTimer::timeout, this, &SimulationWindowPresenter::updateRendering);
}

void SimulationWindowPresenter::initLiveMode(
    std::shared_ptr<std::atomic<SystemSnapshotPtr>> rendering_snapshot,
    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue,
    double simulation_duration) {
    Q_ASSERT(rendering_snapshot != nullptr);
    Q_ASSERT(chart_queue != nullptr);
    mode_ = Mode::Live;

    rendering_snapshot_ = rendering_snapshot;
    chart_queue_ = std::move(chart_queue);

    view_->initLiveMode(simulation_duration);

    render_timer_->start(1000 / target_fps_);
}

void SimulationWindowPresenter::initReplayMode(
    std::shared_ptr<std::atomic<SystemSnapshotPtr>> rendering_snapshot,
    std::shared_ptr<std::vector<double>> timestamps,
    std::shared_ptr<DiagnosticsSeries> diagnostics_series) {
    Q_ASSERT((rendering_snapshot != nullptr && timestamps != nullptr) ||
             diagnostics_series != nullptr);
    mode_ = Mode::Replay;

    rendering_snapshot_ = rendering_snapshot;

    view_->initReplayMode(timestamps, diagnostics_series);

    render_timer_->start(1000 / target_fps_);
}

void SimulationWindowPresenter::updateRendering() {
    if (!rendering_snapshot_) {
        ENKAS_LOG_ERROR("Rendering snapshot is not set. Cannot update rendering.");
        return;
    }

    // If the rendering is faster than the simulation, we do not want to drop frames, but instead
    // pass the nullptr to the view, which must handle it properly.
    auto system_snapshot = rendering_snapshot_->exchange(nullptr, std::memory_order_acq_rel);
    if (system_snapshot) snapshot_count_++;

    // Update the view with the system snapshot
    view_->updateSystemRendering(system_snapshot);
    frame_count_++;

    // Check if it's time to update the FPS display
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed_time = now - last_debug_info_update_time_;
    const auto fps_display_update_interval = std::chrono::milliseconds(200);
    if (elapsed_time < fps_display_update_interval) return;

    // Calculate FPS
    const double elapsed_seconds = std::chrono::duration<double>(elapsed_time).count();
    const int fps = static_cast<int>(frame_count_ / elapsed_seconds);
    const int sps = static_cast<int>(snapshot_count_ / elapsed_seconds);
    view_->updateDebugInfo(fps, sps);
    frame_count_ = 0;     // Reset frame count for the next interval
    snapshot_count_ = 0;  // Reset snapshot count for the next interval
    last_debug_info_update_time_ = now;
}

void SimulationWindowPresenter::updateCharts() {
    if (mode_ != Mode::Live || !chart_queue_) {
        ENKAS_LOG_ERROR("Cannot update charts: not in live mode or chart queue is not set.");
        return;
    }

    DiagnosticsSnapshotPtr diagnostics_snapshot = chart_queue_->popBlocking();
    if (!diagnostics_snapshot) {
        ENKAS_LOG_DEBUG("No diagnostics data available for updating charts.");
        return;
    }

    view_->updateCharts(diagnostics_snapshot);
}
