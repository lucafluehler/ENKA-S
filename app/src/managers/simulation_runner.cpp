#include "simulation_runner.h"

#include <enkas/generation/generator.h>
#include <enkas/logging/logger.h>
#include <enkas/simulation/simulator.h>

#include <QElapsedTimer>
#include <QObject>
#include <QThread>
#include <chrono>
#include <filesystem>
#include <format>
#include <memory>

#include "core/dataflow/snapshot.h"
#include "core/files/file_constants.h"
#include "core/settings/settings.h"
#include "managers/i_simulation_runner.h"
#include "presenters/simulation_window/live/live_simulation_window_presenter.h"
#include "views/simulation_window/live/live_simulation_window.h"
#include "workers/queue_storage_worker.h"
#include "workers/simulation_worker.h"

SimulationRunner::SimulationRunner(const Settings& settings, QObject* parent)
    : ISimulationRunner(parent),
      duration_(settings.get<double>(SettingKey::Duration)),
      save_system_data_(settings.get<bool>(SettingKey::SaveSystemData)),
      save_diagnostics_data_(settings.get<bool>(SettingKey::SaveDiagnosticsData)),
      memory_pools_(std::make_shared<MemoryPools>()),
      outputs_(std::make_shared<SimulationOutputs>()),
      debug_info_timer_(new QTimer(this)) {
    const bool save_settings = settings.get<bool>(SettingKey::SaveSettings);
    ENKAS_LOG_DEBUG("Configuration: SaveSettings={}, SaveSystemData={}, SaveDiagnosticsData={}",
                    save_settings,
                    save_system_data_,
                    save_diagnostics_data_);
    // Create output directory
    setupOutputDir();

    if (save_settings) settings.save(output_dir_ / file_names::settings);

    // Debug Info
    debug_info_ = std::make_shared<LiveDebugInfo>();
    debug_info_->duration = duration_;

    // Populate output queues
    outputs_->rendering_snapshot =
        std::make_shared<std::atomic<SystemSnapshotPtr>>(SystemSnapshotPtr{});
    outputs_->chart_queue = std::make_shared<BlockingQueue<DiagnosticsSnapshotPtr>>(pool_size_);
    debug_info_->chart_queue_capacity = pool_size_;  // Default capacity for the chart queue

    if (save_system_data_) {
        system_file_writer_ = std::make_unique<CsvFileWriter<SystemSnapshot>>(
            output_dir_ / file_names::system, csv_headers::system);
        outputs_->system_storage_queue =
            std::make_shared<BlockingQueue<SystemSnapshotPtr>>(pool_size_);
        setupSystemStorageWorker();
        debug_info_->system_storage_queue_capacity = pool_size_;
    }

    if (save_diagnostics_data_) {
        diagnostics_file_writer_ = std::make_unique<CsvFileWriter<DiagnosticsSnapshot>>(
            output_dir_ / file_names::diagnostics, csv_headers::diagnostics);
        outputs_->diagnostics_storage_queue =
            std::make_shared<BlockingQueue<DiagnosticsSnapshotPtr>>(pool_size_);
        setupDiagnosticsStorageWorker();
        debug_info_->diagnostics_storage_queue_capacity = pool_size_;
    }

    // Simulation Window
    simulation_window_ = new LiveSimulationWindow(debug_info_);
    simulation_window_presenter_ = new LiveSimulationWindowPresenter(
        simulation_window_, outputs_->rendering_snapshot, outputs_->chart_queue, debug_info_);
    connect(simulation_window_,
            &LiveSimulationWindow::fpsChanged,
            simulation_window_presenter_,
            &LiveSimulationWindowPresenter::onFpsChanged);

    // Simulation
    setupSimulationWorker(settings);

    // Debug Info Timer
    connect(debug_info_timer_, &QTimer::timeout, this, &SimulationRunner::updateDebugInfo);
    debug_info_timer_->start(500);  // Update pool and queue size every 500 ms

    ENKAS_LOG_INFO("Simulation runner initialized successfully.");
}

SimulationRunner::~SimulationRunner() {
    ENKAS_LOG_INFO("Simulation runner is being destroyed. Aborting any ongoing processes...");
    aborted_ = true;

    // This presenter runs a worker which relies on memory provided by the simulation runner.
    // We must ensure that the presenter is deleted before the runner goes out of scope.
    if (simulation_window_presenter_) {
        simulation_window_presenter_->deleteLater();
        simulation_window_presenter_ = nullptr;
        ENKAS_LOG_DEBUG("Simulation window presenter deleted.");
    }

    if (simulation_window_) {
        simulation_window_->close();
        simulation_window_->deleteLater();
        simulation_window_ = nullptr;
        ENKAS_LOG_DEBUG("Simulation window deleted.");
    }

    if (simulation_worker_) simulation_worker_->abort();
    if (system_storage_worker_) system_storage_worker_->abort();
    if (diagnostics_storage_worker_) diagnostics_storage_worker_->abort();

    // Join threads
    if (simulation_thread_ && simulation_thread_->isRunning()) {
        simulation_thread_->quit();
        simulation_thread_->wait();
        ENKAS_LOG_DEBUG("Simulation worker thread joined.");
    }

    if (system_storage_thread_ && system_storage_thread_->isRunning()) {
        system_storage_thread_->quit();
        system_storage_thread_->wait();
        ENKAS_LOG_DEBUG("System storage worker thread joined.");
    }

    if (diagnostics_storage_thread_ && diagnostics_storage_thread_->isRunning()) {
        diagnostics_storage_thread_->quit();
        diagnostics_storage_thread_->wait();
        ENKAS_LOG_DEBUG("Diagnostics storage worker thread joined.");
    }

    ENKAS_LOG_INFO("Simulation runner destroyed successfully.");
}

void SimulationRunner::openSimulationWindow() {
    if (!simulation_window_) {
        ENKAS_LOG_ERROR("Simulation window is not initialized. Cannot open it.");
        return;
    }

    simulation_window_->show();
}

void SimulationRunner::receivedGenerationCompleted() {
    emit generationCompleted();
    if (aborted_) return;
    emit requestInitialization();
}

void SimulationRunner::receivedInitializationCompleted() {
    emit initializationCompleted();
    if (aborted_) return;
    emit requestSimulationStart();
}

void SimulationRunner::setupOutputDir() {
    auto now = std::chrono::system_clock::now();
    const auto rounded_now = std::chrono::round<std::chrono::seconds>(now);
    const std::string timestamp = std::format("{:%Y-%m-%d_%H-%M-%S}", rounded_now);

    auto base = std::filesystem::current_path();
    output_dir_ = base / ("enkas_output_" + timestamp);
}

void SimulationRunner::setupSystemStorageWorker() {
    auto save_function = [this](const SystemSnapshotPtr& snapshot) {
        system_file_writer_->write(*snapshot);
    };

    system_storage_worker_ =
        new QueueStorageWorker<SystemSnapshotPtr>(outputs_->system_storage_queue, save_function);

    system_storage_thread_ = new QThread(this);
    system_storage_worker_->moveToThread(system_storage_thread_);

    connect(system_storage_thread_,
            &QThread::started,
            system_storage_worker_,
            &QueueStorageWorkerBase::run);
    connect(system_storage_worker_,
            &QueueStorageWorkerBase::workFinished,
            system_storage_thread_,
            &QThread::quit);
    connect(
        system_storage_thread_, &QThread::finished, system_storage_worker_, &QObject::deleteLater);

    system_storage_thread_->start();

    ENKAS_LOG_INFO("System storage worker thread started.");
}

void SimulationRunner::setupDiagnosticsStorageWorker() {
    auto save_function = [this](const DiagnosticsSnapshotPtr& snapshot) {
        diagnostics_file_writer_->write(*snapshot);
    };

    diagnostics_storage_worker_ = new QueueStorageWorker<DiagnosticsSnapshotPtr>(
        outputs_->diagnostics_storage_queue, save_function);

    diagnostics_storage_thread_ = new QThread(this);
    diagnostics_storage_worker_->moveToThread(diagnostics_storage_thread_);

    connect(diagnostics_storage_thread_,
            &QThread::started,
            diagnostics_storage_worker_,
            &QueueStorageWorkerBase::run);
    connect(diagnostics_storage_worker_,
            &QueueStorageWorkerBase::workFinished,
            diagnostics_storage_thread_,
            &QThread::quit);
    connect(diagnostics_storage_thread_,
            &QThread::finished,
            diagnostics_storage_worker_,
            &QObject::deleteLater);

    diagnostics_storage_thread_->start();

    ENKAS_LOG_INFO("Diagnostics storage worker thread started.");
}

void SimulationRunner::setupSimulationWorker(const Settings& settings) {
    simulation_worker_ = new SimulationWorker(settings, memory_pools_, outputs_, debug_info_);
    simulation_thread_ = new QThread(this);
    simulation_worker_->moveToThread(simulation_thread_);
    connect(simulation_thread_, &QThread::finished, simulation_worker_, &QObject::deleteLater);

    // Signals from Manager to Worker
    connect(this,
            &SimulationRunner::requestGeneration,
            simulation_worker_,
            &SimulationWorker::startGeneration);
    connect(this,
            &SimulationRunner::requestInitialization,
            simulation_worker_,
            &SimulationWorker::startInitialization);
    connect(this,
            &SimulationRunner::requestSimulationStart,
            simulation_worker_,
            &SimulationWorker::runSimulation);

    // Signals from Worker to Manager
    connect(simulation_worker_,
            &SimulationWorker::generationCompleted,
            this,
            &SimulationRunner::receivedGenerationCompleted);
    connect(simulation_worker_,
            &SimulationWorker::initializationCompleted,
            this,
            &SimulationRunner::receivedInitializationCompleted);

    simulation_thread_->start();

    ENKAS_LOG_INFO("Simulation thread started.");
}

void SimulationRunner::updateDebugInfo() {
    if (!debug_info_) {
        ENKAS_LOG_ERROR("Debug info is not initialized. Cannot update.");
        return;
    }

    if (!memory_pools_) {
        ENKAS_LOG_ERROR("Memory pools are not initialized. Cannot update.");
        return;
    }

    // Update memory pool sizes
    if (memory_pools_->system_data_pool) {
        debug_info_->system_data_pool_size = memory_pools_->system_data_pool->size();
    }
    if (memory_pools_->diagnostics_data_pool) {
        debug_info_->diagnostics_data_pool_size = memory_pools_->diagnostics_data_pool->size();
    }
    if (memory_pools_->system_snapshot_pool) {
        debug_info_->system_snapshot_pool_size = memory_pools_->system_snapshot_pool->size();
    }
    if (memory_pools_->diagnostics_snapshot_pool) {
        debug_info_->diagnostics_snapshot_pool_size =
            memory_pools_->diagnostics_snapshot_pool->size();
    }

    // Update queue sizes
    if (outputs_->chart_queue) {
        debug_info_->chart_queue_size = outputs_->chart_queue->size();
    }
    if (outputs_->system_storage_queue) {
        debug_info_->system_storage_queue_size = outputs_->system_storage_queue->size();
    }
    if (outputs_->diagnostics_storage_queue) {
        debug_info_->diagnostics_storage_queue_size = outputs_->diagnostics_storage_queue->size();
    }
}
