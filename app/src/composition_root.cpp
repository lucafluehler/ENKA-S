#include "composition_root.h"

#include <enkas/logging/logger.h>
#include <enkas/logging/sinks.h>

#include <QObject>
#include <QPointer>
#include <memory>

#include "logging/qt_log_sink.h"
#include "presenters/load_simulation/load_simulation_presenter.h"
#include "presenters/main_window/main_window_presenter.h"
#include "presenters/new_simulation/new_simulation_presenter.h"
#include "services/file_parser/file_parser.h"
#include "services/simulation_player_factory/simulation_player_factory.h"
#include "services/simulation_runner_factory/simulation_runner_factory.h"
#include "services/task_runner/concurrent_runner.h"
#include "views/load_simulation_tab/load_simulation_tab.h"
#include "views/logs_tab/logs_tab.h"
#include "views/main_window/main_window.h"
#include "views/new_simulation_tab/new_simulation_tab.h"

CompositionRoot::CompositionRoot() {}
CompositionRoot::~CompositionRoot() = default;

std::unique_ptr<AppLogic> CompositionRoot::compose() {
    new_simulation_tab_ = new NewSimulationTab();
    load_simulation_tab_ = new LoadSimulationTab();
    logs_tab_ = new LogsTab();

    main_window_ =
        std::make_unique<MainWindow>(new_simulation_tab_, load_simulation_tab_, logs_tab_);

    setupServices();
    setupPresenters();
    setupLogging();
    connectSignals();

    return std::make_unique<AppLogic>(std::move(main_window_), std::move(main_window_presenter_));
}

void CompositionRoot::setupServices() {
    concurrent_runner_ = std::make_unique<ConcurrentRunner>();
    file_parser_ = std::make_unique<FileParser>();
    simulation_runner_factory_ = std::make_unique<SimulationRunnerFactory>();
    simulation_player_factory_ = std::make_unique<SimulationPlayerFactory>();
}

void CompositionRoot::setupPresenters() {
    load_simulation_presenter_ = new LoadSimulationPresenter(load_simulation_tab_,
                                                             *file_parser_,
                                                             *concurrent_runner_,
                                                             *simulation_player_factory_,
                                                             load_simulation_tab_);

    new_simulation_presenter_ = new NewSimulationPresenter(new_simulation_tab_,
                                                           *file_parser_,
                                                           *concurrent_runner_,
                                                           *simulation_runner_factory_,
                                                           new_simulation_tab_);

    main_window_presenter_ = std::make_unique<MainWindowPresenter>(
        main_window_.get(), load_simulation_presenter_, new_simulation_presenter_);
}

void CompositionRoot::setupLogging() {
    using namespace enkas::logging;

    auto multi_sink = std::make_shared<MultiSink>();

#if defined(QT_DEBUG)
    multi_sink->addSink(std::make_shared<ConsoleSink>());
#endif

    auto qt_sink = std::make_shared<QtLogSink>(nullptr);
    multi_sink->addSink(qt_sink);

#if defined(QT_DEBUG)
    getLogger().configure(LogLevel::TRACE, std::static_pointer_cast<LogSink>(multi_sink));
#else
    getLogger().configure(LogLevel::INFO, std::static_pointer_cast<LogSink>(multi_sink));
#endif

    if (logs_tab_) {
        QObject::connect(
            qt_sink.get(), &QtLogSink::messageLogged, logs_tab_, &LogsTab::addLogMessage);
        ENKAS_LOG_INFO("UI Logger successfully initialized and connected.");
    } else {
        ENKAS_LOG_WARNING("Could not find LogsTab to connect to QtLogSink.");
    }
}

void CompositionRoot::connectSignals() {
    // Connect NewSimulationTab to its presenter
    QObject::connect(new_simulation_tab_,
                     &NewSimulationTab::checkInitialSystemFile,
                     new_simulation_presenter_,
                     &NewSimulationPresenter::checkInitialSystemFile);
    QObject::connect(new_simulation_tab_,
                     &NewSimulationTab::checkSettingsFile,
                     new_simulation_presenter_,
                     &NewSimulationPresenter::checkSettingsFile);
    QObject::connect(new_simulation_tab_,
                     &NewSimulationTab::requestSimulationStart,
                     new_simulation_presenter_,
                     &NewSimulationPresenter::startSimulation);
    QObject::connect(new_simulation_tab_,
                     &NewSimulationTab::requestSimulationAbort,
                     new_simulation_presenter_,
                     &NewSimulationPresenter::abortSimulation);
    QObject::connect(new_simulation_tab_,
                     &NewSimulationTab::requestOpenSimulationWindow,
                     new_simulation_presenter_,
                     &NewSimulationPresenter::openSimulationWindow);

    // Connect LoadSimulationTab to its presenter
    QObject::connect(load_simulation_tab_,
                     &LoadSimulationTab::requestFilesCheck,
                     load_simulation_presenter_,
                     &LoadSimulationPresenter::checkFiles);
    QObject::connect(load_simulation_tab_,
                     &LoadSimulationTab::playSimulation,
                     load_simulation_presenter_,
                     &LoadSimulationPresenter::playSimulation);

    // Connect MainWindow to its presenter
    QObject::connect(main_window_.get(),
                     &MainWindow::tabSwitched,
                     main_window_presenter_.get(),
                     &MainWindowPresenter::onTabSwitched);
}
