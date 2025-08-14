#include "simulation_player.h"

#include <enkas/logging/logger.h>

#include <QObject>
#include <QTimer>
#include <filesystem>
#include <memory>

#include "core/dataflow/system_ring_buffer.h"
#include "managers/i_simulation_player.h"
#include "presenters/replay_simulation_window_presenter.h"
#include "views/replay_simulation_window/replay_simulation_window.h"
#include "workers/system_buffer_worker.h"

namespace {
// --- Buffer Constants ---
constexpr size_t kSystemRingBufferCapacity = 512;
constexpr size_t kSystemRingBufferRetainCount = 8;

// --- Refresh rates ---
constexpr int kBufferUpdateIntervalMs = 200;
constexpr int kDefaultStepsPerSecond = 30;
}  // namespace

SimulationPlayer::SimulationPlayer(QObject* parent)
    : ISimulationPlayer(parent),
      rendering_snapshot_(std::make_shared<std::atomic<SystemSnapshotPtr>>(SystemSnapshotPtr{})),
      buffer_value_update_timer_(new QTimer(this)),
      step_delay_ms_(1000 / kDefaultStepsPerSecond) {
    connect(buffer_value_update_timer_, &QTimer::timeout, this, [this]() {
        if (system_ring_buffer_ && total_snapshots_count_ > 0) {
            const int buffer_size = system_ring_buffer_->size();
            const int buffer_value = 1000 * buffer_size / total_snapshots_count_;
            simulation_window_->updateBufferValue(buffer_value);
        }
    });
}

SimulationPlayer::~SimulationPlayer() {
    if (simulation_window_) {
        simulation_window_->close();
        simulation_window_->deleteLater();
        simulation_window_ = nullptr;
    }

    if (system_buffer_worker_) system_buffer_worker_->abort();

    // Join buffer thread
    if (system_buffer_thread_ && system_buffer_thread_->isRunning()) {
        system_buffer_thread_->quit();
        system_buffer_thread_->wait();
    }
}

void SimulationPlayer::run(std::optional<ISimulationPlayer::SystemData> system_data,
                           std::optional<ISimulationPlayer::DiagnosticsData> diagnostics_data) {
    const bool has_system_data = system_data.has_value();
    const bool has_diagnostics_data = diagnostics_data.has_value();

    auto simulation_duration = system_data ? system_data->simulation_duration : 0.0;
    auto diagnostics_series = diagnostics_data ? diagnostics_data->diagnostics_series : nullptr;

    setupSimulationWindow(simulation_duration, diagnostics_series);
    simulation_window_->show();

    if (has_system_data) {
        // Setup system buffer worker
        system_ring_buffer_ = std::make_shared<SystemRingBuffer>(kSystemRingBufferCapacity,
                                                                 kSystemRingBufferRetainCount);
        system_file_path_ = system_data->file_path;
        setupSystemBufferWorker();

        // Update buffer value regularly
        total_snapshots_count_ = system_data->total_snapshots_count;
        simulation_window_->updateBufferValue(total_snapshots_count_);
        buffer_value_update_timer_->start(kBufferUpdateIntervalMs);
    }

    setupDataUpdateTimer();
}

void SimulationPlayer::setupSystemBufferWorker() {
    system_buffer_worker_ = new SystemBufferWorker(system_ring_buffer_, system_file_path_);
    system_buffer_thread_ = new QThread(this);
    system_buffer_worker_->moveToThread(system_buffer_thread_);

    connect(
        system_buffer_thread_, &QThread::started, system_buffer_worker_, &SystemBufferWorker::run);
    connect(
        system_buffer_thread_, &QThread::finished, system_buffer_worker_, &QObject::deleteLater);

    system_buffer_thread_->start();

    ENKAS_LOG_INFO("System buffer worker started. Data will be read from: {}",
                   system_file_path_.string());
}

void SimulationPlayer::setupDataUpdateTimer() {
    onTogglePlayback();  // Start playback immediately
}

void SimulationPlayer::onTogglePlayback() {
    is_playing_ = !is_playing_;

    if (is_playing_) {
        onStepForward();
    }
}

void SimulationPlayer::setupSimulationWindow(
    double simulation_duration, const std::shared_ptr<DiagnosticsSeries>& diagnostics_series) {
    simulation_window_ = new ReplaySimulationWindow(simulation_duration > 0.0, diagnostics_series);
    const auto w = simulation_window_;
    connect(w, &ReplaySimulationWindow::togglePlayback, this, &SimulationPlayer::onTogglePlayback);
    connect(w, &ReplaySimulationWindow::stepForward, this, &SimulationPlayer::onStepForward);
    connect(w, &ReplaySimulationWindow::stepBackward, this, &SimulationPlayer::onStepBackward);
    connect(w, &ReplaySimulationWindow::windowClosed, this, [this]() { emit windowClosed(); });
    connect(w, &ReplaySimulationWindow::requestJump, this, &SimulationPlayer::onJump);
    connect(w, &ReplaySimulationWindow::stepsPerSecondChanged, this, [this](int sps) {
        step_delay_ms_ = 1000 / sps;
    });

    simulation_window_presenter_ =
        new ReplaySimulationWindowPresenter(w, rendering_snapshot_, simulation_duration, this);
}

void SimulationPlayer::onStepForward() {
    if (auto system_snapshot = system_ring_buffer_->readForward()) {
        rendering_snapshot_->store(*system_snapshot, std::memory_order_release);
    }

    if (is_playing_) {
        QTimer::singleShot(step_delay_ms_, this, &SimulationPlayer::onStepForward);
    }
}

void SimulationPlayer::onStepBackward() {
    if (auto system_snapshot = system_ring_buffer_->readBackward()) {
        rendering_snapshot_->store(*system_snapshot, std::memory_order_release);
    }

    if (system_buffer_worker_) {
        system_buffer_worker_->requestStepBackward();
    }
}

void SimulationPlayer::onJump(float fraction) {
    if (system_buffer_worker_) {
        system_buffer_worker_->requestJump(fraction);
    }
}
