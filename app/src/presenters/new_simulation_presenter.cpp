#include "new_simulation_presenter.h"

#include <enkas/logging/logger.h>

#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <optional>

#include "core/dataflow/snapshot.h"
#include "core/settings/settings.h"
#include "views/new_simulation_tab/i_new_simulation_view.h"
#include "workers/file_parse_worker.h"
#include "workers/simulation_runner.h"


NewSimulationPresenter::NewSimulationPresenter(INewSimulationView* view, QObject* parent)
    : QObject(parent),
      view_(view),
      preview_timer_(new QTimer(this)),
      progress_timer_(new QTimer(this)) {
    Q_ASSERT(view_ != nullptr);
    // Initialize Timers
    connect(preview_timer_, &QTimer::timeout, this, &NewSimulationPresenter::updatePreview);
    connect(progress_timer_, &QTimer::timeout, this, &NewSimulationPresenter::updateProgress);

    // Initialize file parse worker
    setupFileParseWorker();
}

NewSimulationPresenter::~NewSimulationPresenter() {
    if (file_parse_thread_->isRunning()) {
        file_parse_thread_->quit();
        file_parse_thread_->wait();
    }
}

void NewSimulationPresenter::updatePreview() { view_->updatePreview(); }

void NewSimulationPresenter::updateProgress() {
    if (!simulation_runner_) {
        ENKAS_LOG_ERROR("Simulation manager is not initialized, cannot update progress.");
        return;
    }

    const double time = simulation_runner_->getTime();
    const double duration = simulation_runner_->getDuration();

    view_->updateSimulationProgress(time, duration);
}

void NewSimulationPresenter::checkInitialSystemFile() {
    emit requestOpenSystemFile(view_->getInitialSystemPath());
}

void NewSimulationPresenter::checkSettingsFile() {
    emit requestParseSettings(view_->getSettingsPath());
}

void NewSimulationPresenter::onSettingsParsed(const std::optional<Settings>& settings) {
    view_->processSettings(settings);
}

void NewSimulationPresenter::onInitialSystemParsed(const std::optional<SystemSnapshot>& snapshot) {
    if (snapshot) {
        view_->processInitialSystem(snapshot->data);
    } else {
        view_->processInitialSystem(std::nullopt);
    }
}

void NewSimulationPresenter::onSystemFileOpened(
    const std::optional<std::vector<double>>& timestamps) {
    emit requestInitialSnapshot();
}

void NewSimulationPresenter::startSimulation() {
    if (simulation_runner_) {
        ENKAS_LOG_ERROR("Simulation manager is already initialized.");
        return;
    }

    auto settings = view_->fetchSettings();

    // Initialize Simulation Manager
    simulation_runner_ = new SimulationRunner(settings, this);

    connect(simulation_runner_,
            &SimulationRunner::initializationCompleted,
            this,
            &NewSimulationPresenter::onInitializationCompleted);

    simulation_runner_->startSimulationProcedure();

    // Stop preview timer to halt preview animation
    preview_timer_->stop();
}

void NewSimulationPresenter::onInitializationCompleted() {
    const int fps = 20;  // Frames per second
    progress_timer_->start(1000 / fps);

    view_->showSimulationProgress();
}

void NewSimulationPresenter::abortSimulation() {
    if (!simulation_runner_) {
        ENKAS_LOG_DEBUG("Simulation is not running, nothing to abort.");
        return;
    }

    ENKAS_LOG_INFO("Aborting simulation...");

    simulation_runner_->deleteLater();
    simulation_runner_ = nullptr;

    // Restart preview timer to resume preview animations
    preview_timer_->start();

    // Stop the progress timer
    progress_timer_->stop();

    view_->simulationAborted();
}

void NewSimulationPresenter::openSimulationWindow() {
    if (simulation_runner_) {
        simulation_runner_->openSimulationWindow();
    } else {
        ENKAS_LOG_ERROR("Simulation manager is not initialized, cannot open simulation window.");
    }
}

void NewSimulationPresenter::setupFileParseWorker() {
    file_parse_worker_ = new FileParseWorker();
    file_parse_thread_ = new QThread(this);
    file_parse_worker_->moveToThread(file_parse_thread_);
    connect(file_parse_thread_, &QThread::finished, file_parse_worker_, &QObject::deleteLater);

    connect(this,
            &NewSimulationPresenter::requestParseSettings,
            file_parse_worker_,
            &FileParseWorker::parseSettings);
    connect(this,
            &NewSimulationPresenter::requestOpenSystemFile,
            file_parse_worker_,
            &FileParseWorker::openSystemFile);

    connect(this,
            &NewSimulationPresenter::requestInitialSnapshot,
            file_parse_worker_,
            &FileParseWorker::requestInitialSnapshot);

    connect(file_parse_worker_,
            &FileParseWorker::settingsParsed,
            this,
            &NewSimulationPresenter::onSettingsParsed);
    connect(file_parse_worker_,
            &FileParseWorker::systemFileOpened,
            this,
            &NewSimulationPresenter::onSystemFileOpened);
    connect(file_parse_worker_,
            &FileParseWorker::snapshotReady,
            this,
            &NewSimulationPresenter::onInitialSystemParsed);

    file_parse_thread_->start();
}