#include "simulation_manager.h"

#include <enkas/generation/generator.h>
#include <enkas/simulation/simulator.h>

#include <QElapsedTimer>
#include <QObject>
#include <QThread>
#include <chrono>
#include <filesystem>

#include "core/data_storage_logic.h"
#include "core/settings/settings.h"
#include "core/snapshot.h"
#include "presenters/simulation_window_presenter.h"
#include "simulation_worker.h"
#include "views/simulation_window/simulation_window.h"
#include "workers/queue_storage_worker.h"

SimulationManager::SimulationManager(const Settings& settings, QObject* parent)
    : QObject(parent),
      duration_(settings.get<double>(SettingKey::Duration)),
      save_system_data_(settings.get<bool>(SettingKey::SaveSystemData)),
      save_diagnostics_data_(settings.get<bool>(SettingKey::SaveDiagnosticsData)),
      simulation_window_(new SimulationWindow),
      simulation_worker_(nullptr),
      simulation_thread_(nullptr),
      system_storage_worker_(nullptr),
      system_storage_thread_(nullptr),
      diagnostics_storage_worker_(nullptr),
      diagnostics_storage_thread_(nullptr),
      aborted_(false) {
    // Data storage
    setupOutputDir();

    if (settings.get<bool>(SettingKey::SaveSettings)) {
        DataStorageLogic::saveSettings(output_dir_, settings);
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

    // Simulation Window
    setupSimulationWindow();
}

SimulationManager::~SimulationManager() {
    aborted_ = true;

    if (simulation_window_) {
        simulation_window_->close();
        simulation_window_->deleteLater();
        simulation_window_ = nullptr;
    }

    if (system_storage_worker_) system_storage_worker_->abort();
    if (diagnostics_storage_worker_) diagnostics_storage_worker_->abort();

    // Join storage threads
    if (system_storage_thread_ && system_storage_thread_->isRunning()) {
        system_storage_thread_->quit();
        system_storage_thread_->wait();
    }

    if (diagnostics_storage_thread_ && diagnostics_storage_thread_->isRunning()) {
        diagnostics_storage_thread_->quit();
        diagnostics_storage_thread_->wait();
    }

    // Join simulation thread
    if (simulation_thread_ && simulation_thread_->isRunning()) {
        simulation_thread_->quit();
        simulation_thread_->wait();
    }
}

void SimulationManager::startSimulationProcedere() { emit requestGeneration(); }

double SimulationManager::getDuration() const { return duration_; }

double SimulationManager::getTime() const { return time_; }

void SimulationManager::openSimulationWindow() {
    if (!simulation_window_) return;

    if (simulation_window_presenter_->getMode() == SimulationWindowPresenter::Mode::Uninitialized) {
        simulation_window_presenter_->initLiveMode(&render_queue_slot_, chart_queue_, duration_);
    }

    simulation_window_->show();
}

void SimulationManager::receivedGenerationCompleted() {
    emit generationCompleted();
    if (aborted_) return;
    emit requestInitialization();
}

void SimulationManager::receivedInitializationCompleted() {
    emit initializationCompleted();
    if (aborted_) return;
    emit requestSimulationStep();
}

void SimulationManager::receivedSimulationStep(double time,
                                               SystemSnapshotPtr system_snapshot,
                                               DiagnosticsSnapshotPtr diagnostics_snapshot) {
    performSimulationStep(time, system_snapshot, diagnostics_snapshot);
}

void SimulationManager::performSimulationStep(double time,
                                              SystemSnapshotPtr system_snapshot,
                                              DiagnosticsSnapshotPtr diagnostics_snapshot) {
    emit simulationStep(time);
    time_ = time;

    if (system_snapshot) {
        // Signal to update the render data
        render_queue_slot_.store(system_snapshot, std::memory_order_release);
        emit renderDataStep();

        // Signal to save the system data (if enabled)
        if (save_system_data_) {
            system_storage_queue_->pushBlocking(system_snapshot);
            emit saveRenderData();
        }
    }

    if (diagnostics_snapshot) {
        // Signal to update the charts data
        chart_queue_->pushBlocking(diagnostics_snapshot);
        emit diagnosticsDataStep();

        // Signal to save the diagnostics data (if enabled)
        if (save_diagnostics_data_) {
            diagnostics_storage_queue_->pushBlocking(diagnostics_snapshot);
            emit saveDiagnosticsData();
        }
    }

    if (time <= duration_ && !aborted_) {
        emit requestSimulationStep();
    }
}

void SimulationManager::setupOutputDir() {
    auto now = std::chrono::system_clock::now();
    auto now_t = std::chrono::system_clock::to_time_t(now);
    std::tm local = *std::localtime(&now_t);

    std::ostringstream oss;
    oss << std::put_time(&local, "%Y-%m-%d_%H-%M-%S");
    std::string timestamp = oss.str();

    auto base = std::filesystem::current_path();
    output_dir_ = base / ("enkas_output_" + timestamp);
}

void SimulationManager::setupSystemStorageWorker() {
    system_storage_worker_ = new QueueStorageWorker<SystemSnapshotPtr>(
        system_storage_queue_, [this](auto const& snapshot) {
            DataStorageLogic::saveSystemData(output_dir_, snapshot->time, snapshot->data);
        });
    system_storage_thread_ = new QThread(this);
    system_storage_worker_->moveToThread(system_storage_thread_);
    connect(system_storage_thread_,
            &QThread::started,
            system_storage_worker_,
            &QueueStorageWorkerBase::run);
    system_storage_thread_->start();
}

void SimulationManager::setupDiagnosticsStorageWorker() {
    diagnostics_storage_worker_ = new QueueStorageWorker<DiagnosticsSnapshotPtr>(
        diagnostics_storage_queue_, [this](auto const& snapshot) {
            DataStorageLogic::saveDiagnosticsData(output_dir_, snapshot->time, snapshot->data);
        });
    diagnostics_storage_thread_ = new QThread(this);
    diagnostics_storage_worker_->moveToThread(diagnostics_storage_thread_);
    connect(diagnostics_storage_thread_,
            &QThread::started,
            diagnostics_storage_worker_,
            &QueueStorageWorkerBase::run);
    diagnostics_storage_thread_->start();
}

void SimulationManager::setupSimulationWorker(const Settings& settings) {
    simulation_worker_ = new SimulationWorker(settings);
    simulation_thread_ = new QThread(this);
    simulation_worker_->moveToThread(simulation_thread_);

    // Signals from Manager to Worker
    connect(this,
            &SimulationManager::requestGeneration,
            simulation_worker_,
            &SimulationWorker::startGeneration);
    connect(this,
            &SimulationManager::requestInitialization,
            simulation_worker_,
            &SimulationWorker::startInitialization);
    connect(this,
            &SimulationManager::requestSimulationStep,
            simulation_worker_,
            &SimulationWorker::step);

    // Signals from Worker to Manager
    connect(simulation_worker_,
            &SimulationWorker::generationCompleted,
            this,
            &SimulationManager::receivedGenerationCompleted);
    connect(simulation_worker_,
            &SimulationWorker::initializationCompleted,
            this,
            &SimulationManager::receivedInitializationCompleted);
    connect(simulation_worker_,
            &SimulationWorker::simulationStep,
            this,
            &SimulationManager::receivedSimulationStep);

    simulation_thread_->start();
}

void SimulationManager::setupSimulationWindow() {
    simulation_window_presenter_ = new SimulationWindowPresenter(simulation_window_);

    connect(this,
            &SimulationManager::renderDataStep,
            simulation_window_presenter_,
            &SimulationWindowPresenter::updateRendering);
}
