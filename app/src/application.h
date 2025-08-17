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

/**
 * @class Application
 * @brief The central owner and entry point for the application's main components.
 *
 * This class holds the top-level objects required for the application to run,
 * including the main window, its presenter, and a collection of core services.
 * Its primary responsibility is to manage the lifetime of these components and
 * to start the application's user interface.
 */
class Application {
public:
    /**
     * @struct Services
     * @brief A container for all core, long-lived application services.
     *
     * This struct aggregates shared services that are used across different
     * parts of the application, typically injected into presenters. This
     * centralizes service management and facilitates dependency injection.
     */
    struct Services {
        std::unique_ptr<ITaskRunner> task_runner;
        std::unique_ptr<IFileParser> file_parser;
        std::unique_ptr<ISimulationRunnerFactory> simulation_runner_factory;
        std::unique_ptr<ISimulationPlayerFactory> simulation_player_factory;
        std::shared_ptr<QtLogSink> qt_log_sink;
        std::shared_ptr<enkas::logging::MultiSink> multi_sink;
    };

    /**
     * @brief Constructs the Application instance.
     * @param main_window The main UI window of the application. The Application takes ownership.
     * @param main_window_presenter The main presenter that orchestrates the UI logic. The
     * Application takes ownership.
     * @param services A struct containing all core services. The Application takes ownership.
     */
    Application(std::unique_ptr<MainWindow> main_window,
                std::unique_ptr<MainWindowPresenter> main_window_presenter,
                std::unique_ptr<Services> services);

    /**
     * @brief Starts the application's user interface.
     *
     * This method makes the main window visible, which in turn allows the
     * Qt event loop to begin processing user interactions.
     */
    void run();

private:
    std::unique_ptr<MainWindow> main_window_;
    std::unique_ptr<MainWindowPresenter> main_window_presenter_;
    std::unique_ptr<Services> services_;
};
