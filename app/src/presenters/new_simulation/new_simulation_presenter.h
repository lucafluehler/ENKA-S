#pragma once

#include <enkas/data/system.h>

#include <QObject>
#include <QTimer>
#include <memory>
#include <optional>

#include "core/settings/settings.h"
#include "presenters/new_simulation/i_new_simulation_presenter.h"
#include "services/file_parser/i_file_parser.h"
#include "services/simulation_runner_factory/i_simulation_runner_factory.h"
#include "services/task_runner/i_task_runner.h"
#include "views/new_simulation_tab/i_new_simulation_view.h"

class QThread;
class INewSimulationView;
class SimulationRunner;

/**
 * @brief Manages the new simulation view and handles simulation-related operations.
 */
class NewSimulationPresenter : public QObject, public INewSimulationPresenter {
    Q_OBJECT
public:
    /**
     * @brief Initializes the presenter with the given view and sets up necessary workers and
     * timers.
     * @param view The new simulation view to be managed.
     * @param parser The file parser logic to be used for parsing files.
     * @param runner The task runner to be used for running tasks.
     * @param factory The factory for creating simulation runners.
     * @param parent The parent QObject, defaults to nullptr.
     */
    explicit NewSimulationPresenter(INewSimulationView* view,
                                    IFileParser& parser,
                                    ITaskRunner& runner,
                                    ISimulationRunnerFactory& factory,
                                    QObject* parent = nullptr);
    ~NewSimulationPresenter() override = default;

public slots:
    void active() override;
    void inactive() override;
    void checkInitialSystemFile() override;
    void checkSettingsFile() override;
    void startSimulation() override;
    void abortSimulation() override;
    void openSimulationWindow() override;

private slots:
    void updatePreview();
    void updateProgress();
    void onSettingsParsed(const std::optional<Settings>& settings);
    void onInitialSystemParsed(const std::optional<enkas::data::System>& system);
    void onInitializationCompleted();

private:
    void setupFileParseWorker();

    INewSimulationView* view_;

    QTimer* preview_timer_ = nullptr;
    QTimer* progress_timer_ = nullptr;

    ITaskRunner& runner_;
    IFileParser& parser_;

    ISimulationRunnerFactory& simulation_runner_factory_;
    std::unique_ptr<ISimulationRunner> simulation_runner_;
};
