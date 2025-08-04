#include "simulation_player.h"

#include <enkas/logging/logger.h>

#include <QObject>
#include <QTimer>
#include <filesystem>
#include <memory>
#include <vector>

#include "core/dataflow/system_ring_buffer.h"
#include "presenters/replay_simulation_window_presenter.h"
#include "views/replay_simulation_window/replay_simulation_window.h"
#include "workers/system_buffer_worker.h"

SimulationPlayer::SimulationPlayer(QObject* parent)
    : QObject(parent),
      rendering_snapshot_(std::make_shared<std::atomic<SystemSnapshotPtr>>(SystemSnapshotPtr{})),
      buffer_value_update_timer_(new QTimer(this)) {
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

void SimulationPlayer::run(const std::filesystem::path& system_file_path,
                           std::shared_ptr<std::vector<double>> timestamps,
                           std::shared_ptr<DiagnosticsSeries> diagnostics_series) {
    if ((system_file_path.empty() || !timestamps) && !diagnostics_series) {
        ENKAS_LOG_ERROR("Invalid input. Nothing to replay.");
        return;
    }

    if (!system_file_path.empty()) {
        constexpr size_t capacity = 512;
        constexpr size_t retain_count = 8;
        system_ring_buffer_ = std::make_shared<SystemRingBuffer>(capacity, retain_count);
        system_file_path_ = system_file_path;
        setupSystemBufferWorker();
    }

    setupSimulationWindow(timestamps, diagnostics_series);
    simulation_window_->show();

    setupDataUpdateTimer();

    if (timestamps) {
        total_snapshots_count_ = timestamps->size();
        simulation_window_->updateBufferValue(total_snapshots_count_);
        buffer_value_update_timer_->start(200);
    }
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
    const std::shared_ptr<std::vector<double>>& timestamps,
    const std::shared_ptr<DiagnosticsSeries>& diagnostics_series) {
    simulation_window_ = new ReplaySimulationWindow(timestamps, diagnostics_series);
    const auto w = simulation_window_;
    connect(w, &ReplaySimulationWindow::togglePlayback, this, &SimulationPlayer::onTogglePlayback);
    connect(w, &ReplaySimulationWindow::stepForward, this, &SimulationPlayer::onStepForward);
    connect(w, &ReplaySimulationWindow::stepBackward, this, &SimulationPlayer::onStepBackward);
    connect(w, &ReplaySimulationWindow::windowClosed, this, [this]() { emit windowClosed(); });
    connect(w, &ReplaySimulationWindow::requestJump, this, &SimulationPlayer::onJump);
    connect(w, &ReplaySimulationWindow::stepsPerSecondChanged, this, [this](int sps) {
        step_delay_ms_ = 1000 / sps;
    });

    double simulation_duration = 0.0;
    if (timestamps) {
        simulation_duration = timestamps->back();
    }

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

void SimulationPlayer::onJump(double timestamp) {
    if (system_buffer_worker_) {
        system_buffer_worker_->requestJump(timestamp);
    }
}
