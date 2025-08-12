#include "load_simulation_presenter.h"

#include <QObject>
#include <QThread>
#include <QTimer>
#include <optional>

#include "core/concurrency/run.h"
#include "core/dataflow/snapshot.h"
#include "core/files/file_constants.h"
#include "core/files/file_parse_logic.h"
#include "enkas/data/system.h"
#include "enkas/logging/logger.h"
#include "managers/simulation_player.h"
#include "views/load_simulation_tab/i_load_simulation_view.h"

LoadSimulationPresenter::LoadSimulationPresenter(ILoadSimulationView* view, QObject* parent)
    : QObject(parent), view_(view), preview_timer_(new QTimer(this)) {
    Q_ASSERT(view_ != nullptr);
    // Set up timer for preview updates
    connect(preview_timer_,
            &QTimer::timeout,
            this,
            &LoadSimulationPresenter::updateInitialSystemPreview);
}

LoadSimulationPresenter::~LoadSimulationPresenter() { endSimulationPlayback(); }

void LoadSimulationPresenter::checkFiles() {
    const auto& file_paths = view_->getFilesToCheck();
    if (file_paths.isEmpty()) {
        return;
    }

    for (const auto& file_path : file_paths) {
        if (file_path.endsWith(file_names::settings)) {
            app::concurrency::run(
                this,
                &FileParseLogic::parseSettings,
                [this](const auto& result) { this->onSettingsParsed(result); },
                file_path.toStdString());

        } else if (file_path.endsWith(file_names::system)) {
            system_data_.file_path = file_path.toStdString();

            app::concurrency::run(
                this,
                &FileParseLogic::parseInitialSystem,
                [this](const auto& result) { this->onInitialSystemParsed(result); },
                file_path.toStdString());

            app::concurrency::run(
                this,
                &FileParseLogic::countSnapshots,
                [this](const auto& result) { this->onSnapshotsCounted(result); },
                file_path.toStdString());

            app::concurrency::run(
                this,
                &FileParseLogic::retrieveSimulationDuration,
                [this](const auto& result) { this->onSimulationDurationRetrieved(result); },
                file_path.toStdString());

        } else if (file_path.endsWith(file_names::diagnostics)) {
            app::concurrency::run(
                this,
                &FileParseLogic::parseDiagnosticsSeries,
                [this](const auto& result) { this->onDiagnosticsSeriesParsed(result); },
                file_path.toStdString());
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

void LoadSimulationPresenter::onSnapshotsCounted(const std::optional<int>& count) {
    if (count) system_data_.total_snapshots_count = *count;
}

void LoadSimulationPresenter::onSimulationDurationRetrieved(const std::optional<double>& duration) {
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
