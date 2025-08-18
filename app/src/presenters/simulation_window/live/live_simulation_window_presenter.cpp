#include "live_simulation_window_presenter.h"

#include <QThread>

#include "core/dataflow/latest_value_slot.h"

LiveSimulationWindowPresenter::LiveSimulationWindowPresenter(
    ILiveSimulationWindowView* view,
    std::shared_ptr<LatestValueSlot<SystemSnapshot>> rendering_snapshot,
    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue,
    std::shared_ptr<LiveDebugInfo> debug_info,
    QObject* parent)
    : SimulationWindowPresenter(view, rendering_snapshot, debug_info->duration, parent),
      view_(view),
      chart_queue_(std::move(chart_queue)),
      debug_info_timer_(new QTimer(this)),
      debug_info_(debug_info),
      last_debug_info_update_time_(std::chrono::steady_clock::now()) {
    // Setup chart worker in seperate thread
    chart_worker_ =
        new QueueStorageWorker<DiagnosticsSnapshotPtr>(chart_queue_, [this](auto const& snapshot) {
            QMetaObject::invokeMethod(
                this,
                [this, snapshot] { view_->updateDiagnostics(snapshot); },
                Qt::QueuedConnection);
        });
    chart_thread_ = new QThread(this);
    chart_worker_->moveToThread(chart_thread_);
    connect(chart_thread_, &QThread::started, chart_worker_, &QueueStorageWorkerBase::run);
    connect(chart_thread_, &QThread::finished, chart_worker_, &QObject::deleteLater);
    chart_thread_->start();

    // Setup debug info timer
    connect(
        debug_info_timer_, &QTimer::timeout, this, &LiveSimulationWindowPresenter::updateDebugInfo);
    debug_info_timer_->start(100);
}

LiveSimulationWindowPresenter::~LiveSimulationWindowPresenter() {
    if (chart_worker_) chart_worker_->abort();

    if (chart_thread_ && chart_thread_->isRunning()) {
        chart_thread_->quit();
        chart_thread_->wait();
    }
}

void LiveSimulationWindowPresenter::updateDebugInfo() {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed_time = now - last_debug_info_update_time_;
    last_debug_info_update_time_ = now;
    const double elapsed_seconds = std::chrono::duration<double>(elapsed_time).count();
    const int current_step_count = debug_info_->current_step.load(std::memory_order_relaxed);
    const int steps_since_last_update = current_step_count - previous_step_count_;
    previous_step_count_ = current_step_count;
    const int sps = static_cast<int>(steps_since_last_update / elapsed_seconds);
    view_->updateDebugInfo(sps);
}
