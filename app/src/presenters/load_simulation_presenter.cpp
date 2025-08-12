#include "load_simulation_presenter.h"

#include <QObject>
#include <QThread>
#include <QTimer>
#include <optional>

#include "core/dataflow/snapshot.h"
#include "core/files/file_constants.h"
#include "enkas/data/system.h"
#include "enkas/logging/logger.h"
#include "managers/simulation_player.h"
#include "views/load_simulation_tab/i_load_simulation_view.h"
#include "workers/file_parse_worker.h"

LoadSimulationPresenter::LoadSimulationPresenter(ILoadSimulationView* view, QObject* parent)
    : QObject(parent), view_(view), preview_timer_(new QTimer(this)) {
    Q_ASSERT(view_ != nullptr);
    // Set up timer for preview updates
    connect(preview_timer_,
            &QTimer::timeout,
            this,
            &LoadSimulationPresenter::updateInitialSystemPreview);

    // Initialize file parse worker
    setupFileParseWorker();
}

LoadSimulationPresenter::~LoadSimulationPresenter() {
    endSimulationPlayback();

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
            system_data_.file_path = file_path.toStdString();
            emit requestParseInitialSystem(file_path);
            emit requestCountSnapshots(file_path);
            emit requestRetrieveSimulationDuration(file_path);
        } else if (file_path.endsWith(file_names::diagnostics)) {
            emit requestParseDiagnosticsSeries(file_path);
        }
    }
}

void LoadSimulationPresenter::onSettingsParsed(const std::optional<Settings>& settings) {
    view_->onSettingsParsed(settings);
}

void LoadSimulationPresenter::onInitialSystemParsed(
    const std::optional<enkas::data::System>& system) {
    if (system) {
        view_->onInitialSystemParsed(*system);
    } else {
        view_->onInitialSystemParsed(std::nullopt);
    }
}

void LoadSimulationPresenter::onDiagnosticsSeriesParsed(
    const std::optional<DiagnosticsSeries>& series) {
    view_->onDiagnosticsSeriesParsed(series.has_value());

    if (series) {
        diagnostics_data_.diagnostics_series = std::make_shared<DiagnosticsSeries>(*series);
    }
}

void LoadSimulationPresenter::onSnapshotsCounted(std::optional<int> count) {
    if (count) system_data_.total_snapshots_count = *count;
}

void LoadSimulationPresenter::onSimulationDurationRetrieved(std::optional<double> duration) {
    if (duration) system_data_.simulation_duration = *duration;
}

void LoadSimulationPresenter::playSimulation() {
    if (system_data_.file_path.empty() && diagnostics_data_.diagnostics_series) {
        ENKAS_LOG_ERROR("No valid simulation data to play.");
        return;
    }

    if (simulation_player_) {
        ENKAS_LOG_ERROR("Simulation player is already initialized.");
        return;
    }

    inactive();  // Stop the preview timer

    simulation_player_ = new SimulationPlayer();
    connect(simulation_player_,
            &SimulationPlayer::windowClosed,
            this,
            &LoadSimulationPresenter::endSimulationPlayback);

    simulation_player_->run(system_data_, diagnostics_data_);
}

void LoadSimulationPresenter::endSimulationPlayback() {
    if (simulation_player_) {
        simulation_player_->deleteLater();
        simulation_player_ = nullptr;
    }

    active();  // Restart the preview timer
}

void LoadSimulationPresenter::setupFileParseWorker() {
    file_parse_worker_ = new FileParseWorker();
    file_parse_thread_ = new QThread(this);
    file_parse_worker_->moveToThread(file_parse_thread_);
    connect(file_parse_thread_, &QThread::finished, file_parse_worker_, &QObject::deleteLater);

    connect(this,
            &LoadSimulationPresenter::requestParseSettings,
            file_parse_worker_,
            &FileParseWorker::parseSettings);
    connect(this,
            &LoadSimulationPresenter::requestParseDiagnosticsSeries,
            file_parse_worker_,
            &FileParseWorker::parseDiagnosticsSeries);
    connect(this,
            &LoadSimulationPresenter::requestParseInitialSystem,
            file_parse_worker_,
            &FileParseWorker::parseInitialSystem);
    connect(this,
            &LoadSimulationPresenter::requestCountSnapshots,
            file_parse_worker_,
            &FileParseWorker::countSnapshots);
    connect(this,
            &LoadSimulationPresenter::requestRetrieveSimulationDuration,
            file_parse_worker_,
            &FileParseWorker::retrieveSimulationDuration);

    connect(file_parse_worker_,
            &FileParseWorker::settingsParsed,
            this,
            &LoadSimulationPresenter::onSettingsParsed);
    connect(file_parse_worker_,
            &FileParseWorker::diagnosticsSeriesParsed,
            this,
            &LoadSimulationPresenter::onDiagnosticsSeriesParsed);
    connect(file_parse_worker_,
            &FileParseWorker::initialSystemParsed,
            this,
            &LoadSimulationPresenter::onInitialSystemParsed);
    connect(file_parse_worker_,
            &FileParseWorker::snapshotsCounted,
            this,
            &LoadSimulationPresenter::onSnapshotsCounted);
    connect(file_parse_worker_,
            &FileParseWorker::simulationDurationRetrieved,
            this,
            &LoadSimulationPresenter::onSimulationDurationRetrieved);

    file_parse_thread_->start();
}
