#include "new_simulation_presenter.h"

#include <enkas/logging/logger.h>

#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <optional>

#include "core/concurrency/i_task_runner.h"
#include "core/files/i_file_parse_logic.h"
#include "core/settings/settings.h"
#include "factories/i_simulation_runner_factory.h"
#include "managers/i_simulation_runner.h"
#include "views/new_simulation_tab/i_new_simulation_view.h"

NewSimulationPresenter::NewSimulationPresenter(INewSimulationView* view,
                                               IFileParseLogic* parser,
                                               ITaskRunner* runner,
                                               std::unique_ptr<ISimulationRunnerFactory> factory,
                                               QObject* parent)
    : QObject(parent),
      view_(view),
      parser_(parser),
      runner_(runner),
      simulation_runner_factory_(std::move(factory)),
      preview_timer_(new QTimer(this)),
      progress_timer_(new QTimer(this)) {
    Q_ASSERT(view_ != nullptr);
    Q_ASSERT(parser_ != nullptr);
    Q_ASSERT(runner_ != nullptr);
    Q_ASSERT(simulation_runner_factory_ != nullptr);

    // Initialize Timers
    connect(preview_timer_, &QTimer::timeout, this, &NewSimulationPresenter::updatePreview);
    connect(progress_timer_, &QTimer::timeout, this, &NewSimulationPresenter::updateProgress);
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
    const auto& file_path = view_->getInitialSystemPath();
    runner_->run(
        this,
        [this, path = file_path.toStdString()]() { return parser_->parseInitialSystem(path); },
        [this](const auto& result) { this->onInitialSystemParsed(result); });
}

void NewSimulationPresenter::checkSettingsFile() {
    const auto& file_path = view_->getSettingsPath();
    runner_->run(
        this,
        [this, path = file_path.toStdString()]() { return parser_->parseSettings(path); },
        [this](const auto& result) { this->onSettingsParsed(result); });
}

void NewSimulationPresenter::onSettingsParsed(const std::optional<Settings>& settings) {
    view_->processSettings(settings);
}

void NewSimulationPresenter::onInitialSystemParsed(
    const std::optional<enkas::data::System>& system) {
    if (system) {
        view_->processInitialSystem(*system);
    } else {
        view_->processInitialSystem(std::nullopt);
    }
}

void NewSimulationPresenter::startSimulation() {
    if (simulation_runner_) {
        ENKAS_LOG_ERROR("Simulation manager is already initialized.");
        return;
    }

    auto settings = view_->fetchSettings();

    // Initialize Simulation Manager
    simulation_runner_ = simulation_runner_factory_->create(settings).release();
    simulation_runner_->setParent(this);

    connect(simulation_runner_,
            &ISimulationRunner::initializationCompleted,
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
