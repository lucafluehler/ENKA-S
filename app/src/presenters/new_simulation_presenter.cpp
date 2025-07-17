#include "new_simulation_presenter.h"

#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <optional>

#include "views/new_simulation_tab/i_new_simulation_view.h"
#include "workers/file_parse_worker.h"
#include "workers/simulation_manager.h"

NewSimulationPresenter::NewSimulationPresenter(INewSimulationView* view, QObject* parent)
    : QObject(parent),
      view_(view),
      preview_timer_(new QTimer(this)),
      progress_timer_(new QTimer(this)) {
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

void NewSimulationPresenter::updatePreview() {
    if (!view_) return;
    view_->updatePreview();
}

void NewSimulationPresenter::updateProgress() {
    if (!simulation_manager_) return;

    const double time = simulation_manager_->getTime();
    const double duration = simulation_manager_->getDuration();

    if (view_) {
        view_->updateSimulationProgress(time, duration);
    }
}

void NewSimulationPresenter::checkInitialSystemFile() {
    file_parse_worker_->parseInitialSystem(view_->getInitialSystemPath());
}

void NewSimulationPresenter::checkSettingsFile() {
    file_parse_worker_->parseSettings(view_->getSettingsPath());
}

void NewSimulationPresenter::onSettingsParsed(const std::optional<Settings>& settings) {
    if (view_) {
        view_->processSettings(settings);
    }
}

void NewSimulationPresenter::onInitialSystemParsed(
    const std::optional<enkas::data::System>& system) {
    if (view_) {
        view_->processInitialSystem(system);
    }
}

void NewSimulationPresenter::startSimulation() {
    if (simulation_manager_) return;

    auto settings = view_->fetchSettings();

    // Initialize Simulation Manager
    simulation_manager_ = new SimulationManager(settings);

    connect(simulation_manager_,
            &SimulationManager::initializationCompleted,
            this,
            &NewSimulationPresenter::onInitializationCompleted);

    simulation_manager_->startSimulationProcedere();

    // Stop preview timer to halt preview animation
    preview_timer_->stop();
}

void NewSimulationPresenter::onInitializationCompleted() {
    const int fps = 20;  // Frames per second
    progress_timer_->start(1000 / fps);

    if (view_) {
        view_->showSimulationProgress();
    }
}

void NewSimulationPresenter::abortSimulation() {
    if (simulation_manager_) {
        simulation_manager_ = nullptr;
    }

    // Restart preview timer -> restarts preview animations
    preview_timer_->start();

    if (view_) {
        view_->simulationAborted();
    }
}

void NewSimulationPresenter::openSimulationWindow() {
    if (simulation_manager_) {
        simulation_manager_->openSimulationWindow();
    }
}
