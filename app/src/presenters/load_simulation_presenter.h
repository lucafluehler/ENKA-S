#pragma once

#include <enkas/data/system.h>

#include <QObject>
#include <QTimer>
#include <optional>

#include "core/concurrency/i_task_runner.h"
#include "core/dataflow/snapshot.h"
#include "core/files/i_file_parse_logic.h"
#include "managers/simulation_player.h"
#include "views/load_simulation_tab/i_load_simulation_view.h"

class QThread;
class ILoadSimulationView;

/**
 * @brief LoadSimulationPresenter handles the logic for loading and parsing simulation files.
 * It interacts with the view to update the UI based on the parsed data.
 */
class LoadSimulationPresenter : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs the presenter with a view and initializes the file parsing worker.
     * @param view The view to interact with.
     * @param parser The file parser logic to be used for parsing files.
     * @param runner The task runner to be used for running tasks.
     * @param parent The parent QObject.
     */
    explicit LoadSimulationPresenter(ILoadSimulationView* view,
                                     IFileParseLogic* parser,
                                     ITaskRunner* runner,
                                     QObject* parent = nullptr);
    ~LoadSimulationPresenter();

public slots:
    /**
     * @brief Called when the view requests to check user files.
     */
    void checkFiles();

    /**
     * @brief Called when the tab becomes active.
     * Starts the timer for updating the particle rendering.
     */
    void active() {
        const int fps = 30;
        preview_timer_->start(1000 / fps);
    }

    /**
     * @brief Called when the tab becomes inactive.
     * Stops the timer for updating the particle rendering.
     */
    void inactive() { preview_timer_->stop(); }

    /**
     * @brief Starts the simulation playback with the loaded data.
     */
    void playSimulation();

    /**
     * @brief Ends the simulation playback.
     */
    void endSimulationPlayback();

private slots:
    void updateInitialSystemPreview() { view_->updateInitialSystemPreview(); }
    void onSettingsParsed(const std::optional<Settings>& settings);
    void onInitialSystemParsed(const std::optional<enkas::data::System>& snapshot);
    void onDiagnosticsSeriesParsed(const std::optional<DiagnosticsSeries>& series);
    void onSnapshotsCounted(const std::optional<int>& count);
    void onSimulationDurationRetrieved(const std::optional<double>& duration);

private:
    ILoadSimulationView* view_ = nullptr;

    QTimer* preview_timer_ = nullptr;

    SimulationPlayer::SystemData system_data_;
    SimulationPlayer::DiagnosticsData diagnostics_data_;

    IFileParseLogic* parser_;
    ITaskRunner* runner_;

    SimulationPlayer* simulation_player_ = nullptr;
};
