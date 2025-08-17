#include "composition_root.h"

#include <enkas/logging/logger.h>
#include <enkas/logging/sinks.h>

#include <QObject>
#include <QPointer>
#include <memory>

#include "application.h"
#include "presenters/load_simulation/load_simulation_presenter.h"
#include "presenters/main_window/main_window_presenter.h"
#include "presenters/new_simulation/new_simulation_presenter.h"
#include "services/file_parser/file_parser.h"
#include "services/log_sink/qt_log_sink.h"
#include "services/simulation_player_factory/simulation_player_factory.h"
#include "services/simulation_runner_factory/simulation_runner_factory.h"
#include "services/task_runner/concurrent_runner.h"
#include "views/load_simulation_tab/load_simulation_tab.h"
#include "views/logs_tab/logs_tab.h"
#include "views/main_window/main_window.h"
#include "views/new_simulation_tab/new_simulation_tab.h"

std::unique_ptr<Application> CompositionRoot::compose() {
    // --- Setup Services ---
    auto services = setupServices();

    // --- Setup Views ---
    auto* new_simulation_tab = new NewSimulationTab();
    auto* load_simulation_tab = new LoadSimulationTab();
    auto* logs_tab = new LogsTab();
    auto main_window =
        std::make_unique<MainWindow>(new_simulation_tab, load_simulation_tab, logs_tab);

    // --- Setup Presenters ---
    auto* load_simulation_presenter =
        new LoadSimulationPresenter(load_simulation_tab,
                                    *services->file_parser,
                                    *services->task_runner,
                                    *services->simulation_player_factory,
                                    load_simulation_tab);

    auto* new_simulation_presenter =
        new NewSimulationPresenter(new_simulation_tab,
                                   *services->file_parser,
                                   *services->task_runner,
                                   *services->simulation_runner_factory,
                                   new_simulation_tab);

    auto main_window_presenter = std::make_unique<MainWindowPresenter>(
        main_window.get(), load_simulation_presenter, new_simulation_presenter);

    // --- Setup Logging ---
    setupLogging(*services);

    // --- Connect signals ---
    // Connect log sink to logs tab
    QObject::connect(
        services->qt_log_sink.get(), &QtLogSink::messageLogged, logs_tab, &LogsTab::addLogMessage);

    // Connect NewSimulationTab to its presenter
    QObject::connect(new_simulation_tab,
                     &NewSimulationTab::checkInitialSystemFile,
                     new_simulation_presenter,
                     &NewSimulationPresenter::checkInitialSystemFile);
    QObject::connect(new_simulation_tab,
                     &NewSimulationTab::checkSettingsFile,
                     new_simulation_presenter,
                     &NewSimulationPresenter::checkSettingsFile);
    QObject::connect(new_simulation_tab,
                     &NewSimulationTab::requestSimulationStart,
                     new_simulation_presenter,
                     &NewSimulationPresenter::startSimulation);
    QObject::connect(new_simulation_tab,
                     &NewSimulationTab::requestSimulationAbort,
                     new_simulation_presenter,
                     &NewSimulationPresenter::abortSimulation);
    QObject::connect(new_simulation_tab,
                     &NewSimulationTab::requestOpenSimulationWindow,
                     new_simulation_presenter,
                     &NewSimulationPresenter::openSimulationWindow);

    // Connect LoadSimulationTab to its presenter
    QObject::connect(load_simulation_tab,
                     &LoadSimulationTab::requestFilesCheck,
                     load_simulation_presenter,
                     &LoadSimulationPresenter::checkFiles);
    QObject::connect(load_simulation_tab,
                     &LoadSimulationTab::playSimulation,
                     load_simulation_presenter,
                     &LoadSimulationPresenter::playSimulation);

    // Connect MainWindow to its presenter
    QObject::connect(main_window.get(),
                     &MainWindow::tabSwitched,
                     main_window_presenter.get(),
                     &MainWindowPresenter::onTabSwitched);

    return std::make_unique<Application>(
        std::move(main_window), std::move(main_window_presenter), std::move(services));
}

std::unique_ptr<Application::Services> CompositionRoot::setupServices() {
    auto services = std::make_unique<Application::Services>();
    services->task_runner = std::make_unique<ConcurrentRunner>();
    services->file_parser = std::make_unique<FileParser>();
    services->simulation_runner_factory = std::make_unique<SimulationRunnerFactory>();
    services->simulation_player_factory = std::make_unique<SimulationPlayerFactory>();
    services->qt_log_sink = std::make_shared<QtLogSink>();
    services->multi_sink = std::make_shared<enkas::logging::MultiSink>();
    return services;
}

void CompositionRoot::setupLogging(Application::Services& services) {
    using namespace enkas::logging;

    LogLevel level_to_set;
#if defined(QT_DEBUG)
    services.multi_sink->addSink(std::make_shared<ConsoleSink>());
    level_to_set = LogLevel::TRACE;
#else
    level_to_set = LogLevel::INFO;
#endif

    services.multi_sink->addSink(services.qt_log_sink);

    getLogger().configure(level_to_set, services.multi_sink);
}
