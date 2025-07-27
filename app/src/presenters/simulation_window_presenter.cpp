#include "presenters/simulation_window_presenter.h"

#include <enkas/logging/logger.h>

#include <QObject>
#include <QThread>
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

SimulationWindowPresenter::~SimulationWindowPresenter() {
    if (chart_worker_) chart_worker_->abort();

    if (chart_thread_ && chart_thread_->isRunning()) {
        chart_thread_->quit();
        chart_thread_->wait();
    }
}

void SimulationWindowPresenter::initLiveMode(
    std::shared_ptr<std::atomic<SystemSnapshotPtr>> rendering_snapshot,
    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue,
    std::shared_ptr<LiveDebugInfo> debug_info) {
    Q_ASSERT(rendering_snapshot != nullptr && chart_queue != nullptr && debug_info != nullptr);

    mode_ = Mode::Live;

    rendering_snapshot_ = rendering_snapshot;
    chart_queue_ = std::move(chart_queue);
    debug_info_ = debug_info;

    setupChartWorker();

    view_->initLiveMode(debug_info_);

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

    // Update the view with the system snapshot
    view_->updateSystemRendering(system_snapshot);
    frame_count_++;

    // Check if it's time to update the debug info
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed_time = now - last_debug_info_update_time_;
    const auto fps_display_update_interval = std::chrono::milliseconds(200);
    if (elapsed_time < fps_display_update_interval) return;

    // Calculate FPS
    const double elapsed_seconds = std::chrono::duration<double>(elapsed_time).count();
    const int fps = static_cast<int>(frame_count_ / elapsed_seconds);

    // Calculate SPS (Steps Per Second)
    const int current_step_count = debug_info_->current_step.load(std::memory_order_relaxed);
    const int steps_since_last_update = current_step_count - previous_step_count_;
    previous_step_count_ = current_step_count;
    const int sps = static_cast<int>(steps_since_last_update / elapsed_seconds);

    view_->updateDebugInfo(fps, sps);
    frame_count_ = 0;  // Reset frame count for the next interval
    last_debug_info_update_time_ = now;
}

void SimulationWindowPresenter::updateCharts(DiagnosticsSnapshotPtr diagnostics_snapshot) {
    view_->updateCharts(diagnostics_snapshot);
}

void SimulationWindowPresenter::setupChartWorker() {
    chart_worker_ = new QueueStorageWorker<DiagnosticsSnapshotPtr>(
        chart_queue_, [this](auto const& snapshot) { emit chartDataReady(snapshot); });
    chart_thread_ = new QThread(this);
    chart_worker_->moveToThread(chart_thread_);
    connect(chart_thread_, &QThread::started, chart_worker_, &QueueStorageWorkerBase::run);
    connect(chart_thread_, &QThread::finished, chart_worker_, &QObject::deleteLater);

    // We need to use signals and slots to communicate between the worker and the presenter,
    // as the worker runs in a separate thread.
    connect(this,
            &SimulationWindowPresenter::chartDataReady,
            this,
            &SimulationWindowPresenter::updateCharts);
    chart_thread_->start();
}
