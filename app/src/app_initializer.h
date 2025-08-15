#pragma once

#include <QPointer>
#include <memory>

#include "views/main_window/main_window.h"

class ITaskRunner;
class IFileParser;
class ISimulationRunnerFactory;
class ISimulationPlayerFactory;
class NewSimulationTab;
class LoadSimulationTab;
class LogsTab;
class MainWindowPresenter;
class LoadSimulationPresenter;
class NewSimulationPresenter;

class AppInitializer {
public:
    explicit AppInitializer();
    ~AppInitializer();

    void run();

private:
    void setupServices();
    void setupFactories();
    void setupPresenters();
    void setupLogging();
    void connectSignals();

    std::unique_ptr<ITaskRunner> concurrent_runner_;
    std::unique_ptr<IFileParser> file_parser_;

    std::unique_ptr<ISimulationRunnerFactory> simulation_runner_factory_;
    std::unique_ptr<ISimulationPlayerFactory> simulation_player_factory_;

    std::unique_ptr<MainWindow> main_window_;
    QPointer<NewSimulationTab> new_simulation_tab_;
    QPointer<LoadSimulationTab> load_simulation_tab_;
    QPointer<LogsTab> logs_tab_;

    QPointer<MainWindowPresenter> main_window_presenter_;
    QPointer<LoadSimulationPresenter> load_simulation_presenter_;
    QPointer<NewSimulationPresenter> new_simulation_presenter_;
};
