#pragma once

#include <memory>

#include "views/main_window/main_window.h"

class ITaskRunner;
class IFileParser;
class ISimulationRunnerFactory;
class ISimulationPlayerFactory;
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

    MainWindow main_window_;

    std::unique_ptr<MainWindowPresenter> main_window_presenter_;
    std::unique_ptr<LoadSimulationPresenter> load_simulation_presenter_;
    std::unique_ptr<NewSimulationPresenter> new_simulation_presenter_;
};
