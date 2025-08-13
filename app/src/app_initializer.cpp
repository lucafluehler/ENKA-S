#include "app_initializer.h"

#include <enkas/logging/logger.h>
#include <enkas/logging/sinks.h>

#include <QObject>

#include "core/concurrency/concurrent_runner.h"
#include "core/files/file_parser.h"
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
    setupServices();
    setupPresenters();
    setupLogging();
    connectSignals();

    ENKAS_LOG_INFO("Application starting up...");
    main_window_.show();
}

void AppInitializer::setupServices() {
    concurrent_runner_ = std::make_unique<ConcurrentRunner>();
    file_parser_ = std::make_unique<FileParser>();
}

void AppInitializer::setupPresenters() {
    main_window_presenter_ = std::make_unique<MainWindowPresenter>(&main_window_, &main_window_);

    load_simulation_presenter_ =
        std::make_unique<LoadSimulationPresenter>(main_window_.getLoadSimulationTab(),
                                                  file_parser_.get(),
                                                  concurrent_runner_.get(),
                                                  &main_window_);

    new_simulation_presenter_ =
        std::make_unique<NewSimulationPresenter>(main_window_.getNewSimulationTab(),
                                                 file_parser_.get(),
                                                 concurrent_runner_.get(),
                                                 &main_window_);
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

    auto *logs_tab = main_window_.getLogsTab();
    if (logs_tab) {
        QObject::connect(
            qt_sink.get(), &QtLogSink::messageLogged, logs_tab, &LogsTab::addLogMessage);
        ENKAS_LOG_INFO("UI Logger successfully initialized and connected.");
    } else {
        ENKAS_LOG_WARNING("Could not find LogsTab to connect to QtLogSink.");
    }
}

void AppInitializer::connectSignals() {
    // Connect NewSimulationTab to its presenter
    QObject::connect(main_window_.getNewSimulationTab(),
                     &NewSimulationTab::checkInitialSystemFile,
                     new_simulation_presenter_.get(),
                     &NewSimulationPresenter::checkInitialSystemFile);
    QObject::connect(main_window_.getNewSimulationTab(),
                     &NewSimulationTab::checkSettingsFile,
                     new_simulation_presenter_.get(),
                     &NewSimulationPresenter::checkSettingsFile);
    QObject::connect(main_window_.getNewSimulationTab(),
                     &NewSimulationTab::requestSimulationStart,
                     new_simulation_presenter_.get(),
                     &NewSimulationPresenter::startSimulation);
    QObject::connect(main_window_.getNewSimulationTab(),
                     &NewSimulationTab::requestSimulationAbort,
                     new_simulation_presenter_.get(),
                     &NewSimulationPresenter::abortSimulation);
    QObject::connect(main_window_.getNewSimulationTab(),
                     &NewSimulationTab::requestOpenSimulationWindow,
                     new_simulation_presenter_.get(),
                     &NewSimulationPresenter::openSimulationWindow);

    // Connect LoadSimulationTab to its presenter
    QObject::connect(main_window_.getLoadSimulationTab(),
                     &LoadSimulationTab::requestFilesCheck,
                     load_simulation_presenter_.get(),
                     &LoadSimulationPresenter::checkFiles);
    QObject::connect(main_window_.getLoadSimulationTab(),
                     &LoadSimulationTab::playSimulation,
                     load_simulation_presenter_.get(),
                     &LoadSimulationPresenter::playSimulation);

    // Handle tab switches in the main window
    QObject::connect(&main_window_, &MainWindow::tabSwitched, &main_window_, [this]() {
        auto index = main_window_.getCurrentTabIndex();
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
