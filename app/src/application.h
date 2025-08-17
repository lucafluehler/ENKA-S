#pragma once

#include <enkas/logging/sinks.h>

#include <memory>

#include "presenters/main_window/main_window_presenter.h"
#include "services/file_parser/i_file_parser.h"
#include "services/log_sink/qt_log_sink.h"
#include "services/simulation_player_factory/i_simulation_player_factory.h"
#include "services/simulation_runner_factory/i_simulation_runner_factory.h"
#include "services/task_runner/i_task_runner.h"
#include "views/main_window/main_window.h"

class Application {
public:
    struct Services {
        std::unique_ptr<ITaskRunner> task_runner;
        std::unique_ptr<IFileParser> file_parser;
        std::unique_ptr<ISimulationRunnerFactory> simulation_runner_factory;
        std::unique_ptr<ISimulationPlayerFactory> simulation_player_factory;
        std::shared_ptr<QtLogSink> qt_log_sink;
        std::shared_ptr<enkas::logging::MultiSink> multi_sink;
    };

    Application(std::unique_ptr<MainWindow> main_window,
                std::unique_ptr<MainWindowPresenter> main_window_presenter,
                std::unique_ptr<Services> services);

    void run();

private:
    std::unique_ptr<MainWindow> main_window_;
    std::unique_ptr<MainWindowPresenter> main_window_presenter_;
    std::unique_ptr<Services> services_;
};
