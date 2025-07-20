#include "new_simulation_presenter.h"

#include <enkas/logging/logger.h>

#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <optional>

#include "core/settings/settings.h"
#include "views/new_simulation_tab/i_new_simulation_view.h"
#include "workers/file_parse_worker.h"
#include "workers/simulation_manager.h"

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
    file_parse_worker_ = new FileParseWorker();
    file_parse_thread_ = new QThread(this);
    file_parse_worker_->moveToThread(file_parse_thread_);

    connect(file_parse_worker_,
            &FileParseWorker::settingsParsed,
            this,
            &NewSimulationPresenter::onSettingsParsed);
    connect(file_parse_worker_,
            &FileParseWorker::initialSystemParsed,
            this,
            &NewSimulationPresenter::onInitialSystemParsed);

    file_parse_thread_->start();
}

NewSimulationPresenter::~NewSimulationPresenter() {
    if (file_parse_thread_->isRunning()) {
        file_parse_thread_->quit();
        file_parse_thread_->wait();
    }
}

void NewSimulationPresenter::updatePreview() { view_->updatePreview(); }

void NewSimulationPresenter::updateProgress() {
    if (!simulation_manager_) {
        ENKAS_LOG_ERROR("Simulation manager is not initialized, cannot update progress.");
        return;
    }

    const double time = simulation_manager_->getTime();
    const double duration = simulation_manager_->getDuration();

    view_->updateSimulationProgress(time, duration);
}

void NewSimulationPresenter::checkInitialSystemFile() {
    file_parse_worker_->parseInitialSystem(view_->getInitialSystemPath());
}

void NewSimulationPresenter::checkSettingsFile() {
    file_parse_worker_->parseSettings(view_->getSettingsPath());
}

void NewSimulationPresenter::onSettingsParsed(const std::optional<Settings>& settings) {
    view_->processSettings(settings);
}

void NewSimulationPresenter::onInitialSystemParsed(
    const std::optional<enkas::data::System>& system) {
    view_->processInitialSystem(system);
}

void NewSimulationPresenter::startSimulation() {
    if (simulation_manager_) {
        ENKAS_LOG_ERROR("Simulation manager is already initialized.");
        return;
    }

    auto settings = view_->fetchSettings();

    // Initialize Simulation Manager
    simulation_manager_ = new SimulationManager(settings, this);

    connect(simulation_manager_,
            &SimulationManager::initializationCompleted,
            this,
            &NewSimulationPresenter::onInitializationCompleted);

    simulation_manager_->startSimulationProcedure();

    // Stop preview timer to halt preview animation
    preview_timer_->stop();
}

void NewSimulationPresenter::onInitializationCompleted() {
    const int fps = 20;  // Frames per second
    progress_timer_->start(1000 / fps);

    view_->showSimulationProgress();
}

void NewSimulationPresenter::abortSimulation() {
    if (!simulation_manager_) {
        ENKAS_LOG_DEBUG("Simulation is not running, nothing to abort.");
        return;
    }

    ENKAS_LOG_INFO("Aborting simulation...");

    simulation_manager_->deleteLater();
    simulation_manager_ = nullptr;

    // Restart preview timer to resume preview animations
    preview_timer_->start();

    // Stop the progress timer
    progress_timer_->stop();

    view_->simulationAborted();
}

void NewSimulationPresenter::openSimulationWindow() {
    if (simulation_manager_) {
        simulation_manager_->openSimulationWindow();
    } else {
        ENKAS_LOG_ERROR("Simulation manager is not initialized, cannot open simulation window.");
    }
}
