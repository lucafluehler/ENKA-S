#include <enkas/logging/logger.h>
#include <enkas/logging/sinks.h>
#include <qobject.h>

#include <QApplication>

#include "logging/qt_log_sink.h"
#include "presenters/main_window_presenter.h"
#include "views/logs_tab/logs_tab.h"
#include "views/main_window/main_window.h"

void setupLogging(QObject *parent) {
    using namespace enkas::logging;

    auto multi_sink = std::make_shared<enkas::logging::MultiSink>();

#if defined(QT_DEBUG)
    multi_sink->addSink(std::make_shared<enkas::logging::ConsoleSink>());
#endif

    auto qt_sink = std::make_shared<QtLogSink>(parent);
    multi_sink->addSink(qt_sink);

#if defined(QT_DEBUG)
    enkas::logging::getLogger().configure(enkas::logging::LogLevel::TRACE, multi_sink);
#else
    enkas::logging::getLogger().configure(enkas::logging::LogLevel::INFO, multi_sink);
#endif

    auto *logs_tab = parent->findChild<LogsTab *>();
    if (logs_tab) {
        QObject::connect(
            qt_sink.get(), &QtLogSink::messageLogged, logs_tab, &LogsTab::addLogMessage);
        ENKAS_LOG_INFO("UI Logger successfully initialized and connected.");
    } else {
        ENKAS_LOG_WARNING("Could not find LogsTab to connect to QtLogSink.");
    }
}

int main(int argc, char *argv[]) {
    qputenv("QT_QPA_PLATFORM", QByteArray("windows:darkmode=0"));
    QApplication app(argc, argv);

    MainWindow main_window;
    MainWindowPresenter presenter(&main_window);

    setupLogging(&main_window);

    ENKAS_LOG_INFO("Application starting up...");

    main_window.show();
    return app.exec();
}
