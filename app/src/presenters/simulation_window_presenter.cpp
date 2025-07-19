#include "presenters/simulation_window_presenter.h"

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
      render_timer_(new QTimer(this)) {
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
    const int fps = 120;
    render_timer_->start(1000 / fps);
}

void SimulationWindowPresenter::initFileMode(const QString& system_file_path,
                                             const QString& diagnostics_file_path) {
    mode_ = Mode::File;
    // TODO
}

void SimulationWindowPresenter::updateRendering() {
    // Retrieve the latest system snapshot from the render queue
    if (mode_ == Mode::Uninitialized) return;

    auto system_snapshot = render_queue_slot_->load(std::memory_order_acquire);
    if (!system_snapshot) return;

    // Calculate FPS based on the time since the last render
    const auto now = std::chrono::steady_clock::now();
    const double fps = 1.0 / std::chrono::duration<double>(now - last_render_time_).count();
    last_render_time_ = now;

    // Update the view with the system snapshot and FPS
    view_->updateSystemRendering(system_snapshot, simulation_duration_, fps);
}
