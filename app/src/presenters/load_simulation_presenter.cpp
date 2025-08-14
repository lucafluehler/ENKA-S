#include "load_simulation_presenter.h"

#include <enkas/logging/logger.h>

#include <QObject>
#include <QThread>
#include <QTimer>
#include <optional>

#include "core/concurrency/i_task_runner.h"
#include "core/dataflow/snapshot.h"
#include "core/files/file_constants.h"
#include "core/files/i_file_parse_logic.h"
#include "factories/i_simulation_player_factory.h"
#include "managers/i_simulation_player.h"
#include "views/load_simulation_tab/i_load_simulation_view.h"

LoadSimulationPresenter::LoadSimulationPresenter(ILoadSimulationView* view,
                                                 IFileParseLogic* parser,
                                                 ITaskRunner* runner,
                                                 std::unique_ptr<ISimulationPlayerFactory> factory,
                                                 QObject* parent)
    : QObject(parent),
      view_(view),
      parser_(parser),
      runner_(runner),
      simulation_player_factory_(std::move(factory)),
      preview_timer_(new QTimer(this)) {
    Q_ASSERT(view_ != nullptr);
    Q_ASSERT(parser_ != nullptr);
    Q_ASSERT(runner_ != nullptr);
    Q_ASSERT(simulation_player_factory_ != nullptr);

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
            runner_->run(
                this,
                [this, path = file_path.toStdString()]() { return parser_->parseSettings(path); },
                [this](const auto& result) { this->onSettingsParsed(result); });

        } else if (file_path.endsWith(file_names::system)) {
            system_data_.file_path = file_path.toStdString();

            runner_->run(
                this,
                [this, path = file_path.toStdString()]() {
                    return parser_->parseInitialSystem(path);
                },
                [this](const auto& result) { this->onInitialSystemParsed(result); });

            runner_->run(
                this,
                [this, path = file_path.toStdString()]() { return parser_->countSnapshots(path); },
                [this](const auto& result) { this->onSnapshotsCounted(result); });

            runner_->run(
                this,
                [this, path = file_path.toStdString()]() {
                    return parser_->retrieveSimulationDuration(path);
                },
                [this](const auto& result) { this->onSimulationDurationRetrieved(result); });
        } else if (file_path.endsWith(file_names::diagnostics)) {
            runner_->run(
                this,
                [this, path = file_path.toStdString()]() {
                    return parser_->parseDiagnosticsSeries(path);
                },
                [this](const auto& result) { this->onDiagnosticsSeriesParsed(result); });
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

    simulation_player_ = simulation_player_factory_->create();

    connect(simulation_player_.get(),
            &ISimulationPlayer::windowClosed,
            this,
            &LoadSimulationPresenter::endSimulationPlayback);

    simulation_player_->run(system_data_, diagnostics_data_);
}

void LoadSimulationPresenter::endSimulationPlayback() {
    simulation_player_.reset();
    active();  // Restart the preview timer
}
