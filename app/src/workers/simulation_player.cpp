#include "simulation_player.h"

#include <enkas/logging/logger.h>

#include <QObject>
#include <QTimer>
#include <filesystem>
#include <memory>
#include <vector>

#include "core/dataflow/system_ring_buffer.h"
#include "presenters/simulation_window_presenter.h"
#include "views/simulation_window/simulation_window.h"
#include "workers/system_buffer_worker.h"

SimulationPlayer::SimulationPlayer(QObject* parent)
    : QObject(parent),
      simulation_window_(new SimulationWindow()),
      rendering_snapshot_(std::make_shared<std::atomic<SystemSnapshotPtr>>(SystemSnapshotPtr{})) {
    // Setup simulation window
    const auto& win = simulation_window_;
    connect(win, &SimulationWindow::togglePlayback, this, &SimulationPlayer::onTogglePlayback);
    connect(win, &SimulationWindow::stepForward, this, &SimulationPlayer::onStepForward);
    connect(win, &SimulationWindow::stepBackward, this, &SimulationPlayer::onStepBackward);
    connect(win, &SimulationWindow::windowClosed, this, [this]() { emit windowClosed(); });
    connect(win, &SimulationWindow::stepsPerSecondChanged, this, [this](int sps) {
        step_delay_ms_ = 1000 / sps;
    });
    simulation_window_presenter_ = new SimulationWindowPresenter(win, this);
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

    simulation_window_presenter_->initReplayMode(
        rendering_snapshot_, timestamps, diagnostics_series);
    simulation_window_->show();

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