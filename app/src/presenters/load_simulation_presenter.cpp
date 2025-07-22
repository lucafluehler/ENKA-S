#include "load_simulation_presenter.h"

#include <QObject>
#include <QThread>
#include <QTimer>
#include <optional>

#include "core/files/file_constants.h"
#include "core/snapshot.h"
#include "enkas/logging/logger.h"
#include "views/load_simulation_tab/i_load_simulation_view.h"
#include "workers/file_parse_worker.h"
#include "workers/simulation_player.h"

LoadSimulationPresenter::LoadSimulationPresenter(ILoadSimulationView* view, QObject* parent)
    : QObject(parent), view_(view), preview_timer_(new QTimer(this)) {
    Q_ASSERT(view_ != nullptr);
    // Set up timer for preview updates
    connect(preview_timer_,
            &QTimer::timeout,
            this,
            &LoadSimulationPresenter::updateInitialSystemPreview);

    // Initialize file parse worker
    file_parse_worker_ = new FileParseWorker();
    file_parse_thread_ = new QThread(this);
    file_parse_worker_->moveToThread(file_parse_thread_);
    connect(file_parse_thread_, &QThread::finished, file_parse_worker_, &QObject::deleteLater);

    connect(this,
            &LoadSimulationPresenter::requestParseSettings,
            file_parse_worker_,
            &FileParseWorker::parseSettings);

    connect(this,
            &LoadSimulationPresenter::requestOpenSystemFile,
            file_parse_worker_,
            &FileParseWorker::openSystemFile);

    connect(this,
            &LoadSimulationPresenter::requestParseDiagnosticsSeries,
            file_parse_worker_,
            &FileParseWorker::parseDiagnosticsSeries);

    connect(this,
            &LoadSimulationPresenter::requestInitialSnapshot,
            file_parse_worker_,
            &FileParseWorker::requestInitialSnapshot);

    connect(file_parse_worker_,
            &FileParseWorker::settingsParsed,
            this,
            &LoadSimulationPresenter::onSettingsParsed);
    connect(file_parse_worker_,
            &FileParseWorker::diagnosticsSeriesParsed,
            this,
            &LoadSimulationPresenter::onDiagnosticsSeriesParsed);
    connect(file_parse_worker_,
            &FileParseWorker::snapshotReady,
            this,
            &LoadSimulationPresenter::onInitialSystemParsed);
    connect(file_parse_worker_,
            &FileParseWorker::systemFileOpened,
            this,
            &LoadSimulationPresenter::onSystemFileOpened);

    file_parse_thread_->start();
}

LoadSimulationPresenter::~LoadSimulationPresenter() {
    if (file_parse_thread_) {
        file_parse_thread_->quit();
        file_parse_thread_->wait();
    }
}

void LoadSimulationPresenter::checkFiles() {
    const auto& file_paths = view_->getFilesToCheck();
    if (file_paths.isEmpty()) {
        return;
    }

    for (const auto& file_path : file_paths) {
        if (file_path.endsWith(file_names::settings)) {
            emit requestParseSettings(file_path);
        } else if (file_path.endsWith(file_names::system)) {
            system_file_path_ = file_path.toStdString();
            emit requestOpenSystemFile(file_path);
        } else if (file_path.endsWith(file_names::diagnostics)) {
            emit requestParseDiagnosticsSeries(file_path);
        }
    }
}

void LoadSimulationPresenter::onSettingsParsed(const std::optional<Settings>& settings) {
    view_->onSettingsParsed(settings);
}

void LoadSimulationPresenter::onInitialSystemParsed(const std::optional<SystemSnapshot>& snapshot) {
    if (snapshot) {
        view_->onInitialSystemParsed(snapshot->data);
    } else {
        view_->onInitialSystemParsed(std::nullopt);
    }
}

void LoadSimulationPresenter::onSystemFileOpened(
    const std::optional<std::vector<double>>& timestamps) {
    if (timestamps) {
        timestamps_ = std::make_shared<std::vector<double>>(*timestamps);
    }
    emit requestInitialSnapshot();
}

void LoadSimulationPresenter::onDiagnosticsSeriesParsed(
    const std::optional<DiagnosticsSeries>& series) {
    view_->onDiagnosticsSeriesParsed(series.has_value());

    if (series) {
        diagnostics_series_ = std::make_shared<DiagnosticsSeries>(*series);
    }
}

void LoadSimulationPresenter::playSimulation() {
    if ((!timestamps_ || system_file_path_.empty()) && !diagnostics_series_) {
        ENKAS_LOG_ERROR("No valid simulation data to play.");
        return;
    }

    if (simulation_player_) {
        ENKAS_LOG_ERROR("Simulation player is already initialized.");
        return;
    }

    inactive();  // Stop the preview timer

    simulation_player_ = new SimulationPlayer();
    simulation_player_->run(system_file_path_, timestamps_, diagnostics_series_);
}

void LoadSimulationPresenter::endSimulationPlayback() {
    if (simulation_player_) {
        simulation_player_->deleteLater();
        simulation_player_ = nullptr;
    }

    active();  // Restart the preview timer
}
