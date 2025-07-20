#include "presenters/simulation_window_presenter.h"

#include <enkas/logging/logger.h>

#include <QObject>
#include <QTimer>
#include <atomic>
#include <chrono>
#include <memory>

#include "core/blocking_queue.h"
#include "core/snapshot.h"
#include "views/simulation_window/i_simulation_window_view.h"

SimulationWindowPresenter::SimulationWindowPresenter(ISimulationWindowView* view, QObject* parent)
    : QObject(parent),
      view_(view),
      mode_(Mode::Uninitialized),
      simulation_duration_(0.0),
      render_timer_(new QTimer(this)),
      last_fps_update_time_(std::chrono::steady_clock::now()) {
    connect(render_timer_, &QTimer::timeout, this, &SimulationWindowPresenter::updateRendering);
}

void SimulationWindowPresenter::initLiveMode(
    std::atomic<SystemSnapshotPtr>* render_queue_slot,
    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue,
    double simulation_duration) {
    Q_ASSERT(render_queue_slot != nullptr);
    Q_ASSERT(chart_queue != nullptr);
    mode_ = Mode::Live;

    render_queue_slot_ = render_queue_slot;
    chart_queue_ = std::move(chart_queue);
    simulation_duration_ = simulation_duration;

    view_->initLiveMode();

    // Start the rendering timer
    render_timer_->start(1000 / target_fps_);
}

void SimulationWindowPresenter::initFileMode(const QString& system_file_path,
                                             const QString& diagnostics_file_path) {
    mode_ = Mode::File;
    // TODO
}

void SimulationWindowPresenter::updateRendering() {
    if (mode_ == Mode::Uninitialized) {
        ENKAS_LOG_ERROR("SimulationWindowPresenter is not initialized. Cannot update rendering.");
        return;
    }

    if (render_queue_slot_ == nullptr) {
        ENKAS_LOG_ERROR("Render queue slot is not set. Cannot update rendering.");
        return;
    }

    // Retrieve the latest system snapshot from the render queue
    auto system_snapshot = render_queue_slot_->load(std::memory_order_acquire);
    if (!system_snapshot) {
        ENKAS_LOG_DEBUG("No system snapshot available for rendering.");
        return;
    }

    // Update the view with the system snapshot
    view_->updateSystemRendering(system_snapshot, simulation_duration_);
    frame_count_++;

    // Check if it's time to update the FPS display
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed_time = now - last_fps_update_time_;
    const auto fps_display_update_interval = std::chrono::milliseconds(200);
    if (elapsed_time < fps_display_update_interval) return;

    // Calculate FPS
    const double elapsed_seconds = std::chrono::duration<double>(elapsed_time).count();
    const int fps = static_cast<int>(frame_count_ / elapsed_seconds);
    view_->updateFPS(fps);
    frame_count_ = 0;  // Reset frame count for the next interval
    last_fps_update_time_ = now;
}
