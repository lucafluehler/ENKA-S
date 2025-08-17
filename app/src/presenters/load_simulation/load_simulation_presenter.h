#pragma once

#include <enkas/data/system.h>

#include <QObject>
#include <QTimer>
#include <memory>
#include <optional>

#include "core/concurrency/i_task_runner.h"
#include "core/dataflow/snapshot.h"
#include "core/files/i_file_parser.h"
#include "factories/i_simulation_player_factory.h"
#include "managers/i_simulation_player.h"
#include "presenters/load_simulation/i_load_simulation_presenter.h"
#include "views/load_simulation_tab/i_load_simulation_view.h"

class QThread;
class ILoadSimulationView;

/**
 * @brief LoadSimulationPresenter handles the logic for loading and parsing simulation files.
 * It interacts with the view to update the UI based on the parsed data.
 */
class LoadSimulationPresenter : public QObject, public ILoadSimulationPresenter {
    Q_OBJECT
public:
    /**
     * @brief Constructs the presenter with a view and initializes the file parsing worker.
     * @param view The view to interact with.
     * @param parser The file parser logic to be used for parsing files.
     * @param runner The task runner to be used for running tasks.
     * @param factory The simulation player factory to be used for creating simulation players.
     * @param parent The parent QObject.
     */
    explicit LoadSimulationPresenter(ILoadSimulationView* view,
                                     IFileParser& parser,
                                     ITaskRunner& runner,
                                     ISimulationPlayerFactory& factory,
                                     QObject* parent = nullptr);
    ~LoadSimulationPresenter();

public slots:
    void checkFiles() override;
    void active() override {
        const int fps = 30;
        preview_timer_->start(1000 / fps);
    }
    void inactive() override { preview_timer_->stop(); }
    void playSimulation() override;
    void endSimulationPlayback() override;

private slots:
    void updateInitialSystemPreview() { view_->updateInitialSystemPreview(); }
    void onSettingsParsed(const std::optional<Settings>& settings);
    void onInitialSystemParsed(const std::optional<enkas::data::System>& snapshot);
    void onDiagnosticsSeriesParsed(const std::optional<DiagnosticsSeries>& series);
    void onSnapshotsCounted(const std::optional<int>& count);
    void onSimulationDurationRetrieved(const std::optional<double>& duration);

private:
    ILoadSimulationView* view_;

    QTimer* preview_timer_ = nullptr;

    ISimulationPlayer::SystemData system_data_;
    ISimulationPlayer::DiagnosticsData diagnostics_data_;

    IFileParser& parser_;
    ITaskRunner& runner_;

    ISimulationPlayerFactory& simulation_player_factory_;
    std::unique_ptr<ISimulationPlayer> simulation_player_;
};
