#include "app_initializer.h"

#include <enkas/logging/logger.h>
#include <enkas/logging/sinks.h>

#include <QObject>

#include "core/concurrency/concurrent_runner.h"
#include "core/files/file_parser.h"
#include "factories/simulation_player_factory.h"
#include "factories/simulation_runner_factory.h"
#include "logging/qt_log_sink.h"
#include "presenters/load_simulation_presenter.h"
#include "presenters/main_window_presenter.h"
#include "presenters/new_simulation_presenter.h"
#include "views/load_simulation_tab/load_simulation_tab.h"
#include "views/logs_tab/logs_tab.h"
#include "views/main_window/main_window.h"
#include "views/new_simulation_tab/new_simulation_tab.h"

AppInitializer::AppInitializer() {}
AppInitializer::~AppInitializer() = default;

void AppInitializer::run() {
    new_simulation_tab_ = new NewSimulationTab();
    load_simulation_tab_ = new LoadSimulationTab();
    logs_tab_ = new LogsTab();

    main_window_ =
        std::make_unique<MainWindow>(new_simulation_tab_, load_simulation_tab_, logs_tab_);

    setupServices();
    setupFactories();
    setupPresenters();
    setupLogging();
    connectSignals();

    ENKAS_LOG_INFO("Application starting up...");
    main_window_->show();
}

void AppInitializer::setupServices() {
    concurrent_runner_ = std::make_unique<ConcurrentRunner>();
    file_parser_ = std::make_unique<FileParser>();
}

void AppInitializer::setupFactories() {
    simulation_runner_factory_ = std::make_unique<SimulationRunnerFactory>();
    simulation_player_factory_ = std::make_unique<SimulationPlayerFactory>();
}

void AppInitializer::setupPresenters() {
    main_window_presenter_ = new MainWindowPresenter(main_window_.get(), main_window_.get());

    load_simulation_presenter_ = new LoadSimulationPresenter(load_simulation_tab_,
                                                             file_parser_.get(),
                                                             concurrent_runner_.get(),
                                                             std::move(simulation_player_factory_),
                                                             main_window_.get());

    new_simulation_presenter_ = new NewSimulationPresenter(new_simulation_tab_,
                                                           file_parser_.get(),
                                                           concurrent_runner_.get(),
                                                           std::move(simulation_runner_factory_),
                                                           main_window_.get());
}

void AppInitializer::setupLogging() {
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

void AppInitializer::connectSignals() {
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

    // Handle tab switches in the main window
    QObject::connect(main_window_.get(), &MainWindow::tabSwitched, main_window_.get(), [this]() {
        auto index = main_window_->getCurrentTabIndex();
        if (index == 2) {
            load_simulation_presenter_->active();
            new_simulation_presenter_->inactive();
        } else if (index == 1) {
            load_simulation_presenter_->inactive();
            new_simulation_presenter_->active();
        } else {
            load_simulation_presenter_->inactive();
            new_simulation_presenter_->inactive();
        }
    });
}
