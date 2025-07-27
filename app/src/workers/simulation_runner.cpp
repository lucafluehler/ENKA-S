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

#include "core/dataflow/snapshot.h"
#include "core/files/data_storage_logic.h"
#include "core/settings/settings.h"
#include "presenters/simulation_window_presenter.h"
#include "simulation_worker.h"
#include "views/simulation_window/simulation_window.h"
#include "workers/queue_storage_worker.h"

SimulationRunner::SimulationRunner(const Settings& settings, QObject* parent)
    : QObject(parent),
      duration_(settings.get<double>(SettingKey::Duration)),
      save_system_data_(settings.get<bool>(SettingKey::SaveSystemData)),
      save_diagnostics_data_(settings.get<bool>(SettingKey::SaveDiagnosticsData)),
      simulation_window_(new SimulationWindow),
      rendering_snapshot_(std::make_shared<std::atomic<SystemSnapshotPtr>>(SystemSnapshotPtr{})),
      chart_queue_(std::make_shared<BlockingQueue<DiagnosticsSnapshotPtr>>(512)),
      system_storage_queue_(std::make_shared<BlockingQueue<SystemSnapshotPtr>>(512)),
      diagnostics_storage_queue_(std::make_shared<BlockingQueue<DiagnosticsSnapshotPtr>>(512)) {
    const bool save_settings = settings.get<bool>(SettingKey::SaveSettings);
    ENKAS_LOG_DEBUG("Configuration: SaveSettings={}, SaveSystemData={}, SaveDiagnosticsData={}",
                    save_settings,
                    save_system_data_,
                    save_diagnostics_data_);

    // Simulation Window
    simulation_window_presenter_ = new SimulationWindowPresenter(simulation_window_, this);
    simulation_window_presenter_->initLiveMode(rendering_snapshot_, chart_queue_, duration_);

    // Data storage
    setupOutputDir();

    if (save_settings) {
        DataStorageLogic::saveSettings(output_dir_, settings);
        ENKAS_LOG_INFO("Settings saved to: {}", output_dir_.string());
    }

    if (save_system_data_) {
        system_storage_queue_ = std::make_shared<BlockingQueue<SystemSnapshotPtr>>(512);
        setupSystemStorageWorker();
    }

    if (save_diagnostics_data_) {
        diagnostics_storage_queue_ = std::make_shared<BlockingQueue<DiagnosticsSnapshotPtr>>(512);
        setupDiagnosticsStorageWorker();
    }

    // Simulation
    setupSimulationWorker(settings);

    ENKAS_LOG_INFO("Simulation runner initialized successfully.");
}

SimulationRunner::~SimulationRunner() {
    ENKAS_LOG_INFO("Simulation runner is being destroyed. Aborting any ongoing processes...");
    aborted_ = true;

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
    system_storage_worker_ = new QueueStorageWorker<SystemSnapshotPtr>(
        system_storage_queue_, [this](auto const& snapshot) {
            DataStorageLogic::saveSystemData(output_dir_, snapshot->time, *snapshot->data);
        });
    system_storage_thread_ = new QThread(this);
    system_storage_worker_->moveToThread(system_storage_thread_);
    connect(system_storage_thread_,
            &QThread::started,
            system_storage_worker_,
            &QueueStorageWorkerBase::run);
    connect(
        system_storage_thread_, &QThread::finished, system_storage_worker_, &QObject::deleteLater);

    system_storage_thread_->start();

    ENKAS_LOG_INFO("System storage worker started. Data will be saved to: {}",
                   output_dir_.string());
}

void SimulationRunner::setupDiagnosticsStorageWorker() {
    diagnostics_storage_worker_ = new QueueStorageWorker<DiagnosticsSnapshotPtr>(
        diagnostics_storage_queue_, [this](auto const& snapshot) {
            DataStorageLogic::saveDiagnosticsData(output_dir_, snapshot->time, *snapshot->data);
        });
    diagnostics_storage_thread_ = new QThread(this);
    diagnostics_storage_worker_->moveToThread(diagnostics_storage_thread_);
    connect(diagnostics_storage_thread_,
            &QThread::started,
            diagnostics_storage_worker_,
            &QueueStorageWorkerBase::run);
    connect(diagnostics_storage_thread_,
            &QThread::finished,
            diagnostics_storage_worker_,
            &QObject::deleteLater);
    diagnostics_storage_thread_->start();

    ENKAS_LOG_INFO("Diagnostics storage worker started. Data will be saved to: {}",
                   output_dir_.string());
}

void SimulationRunner::setupSimulationWorker(const Settings& settings) {
    simulation_worker_ = new SimulationWorker(settings,
                                              rendering_snapshot_,
                                              chart_queue_,
                                              system_storage_queue_,
                                              diagnostics_storage_queue_);
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
