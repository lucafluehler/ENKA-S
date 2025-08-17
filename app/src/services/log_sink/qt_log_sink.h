#pragma once

#include <enkas/logging/logger.h>

#include <QObject>
#include <QString>
#include <string_view>

/**
 * @brief QtLogSink is a log sink that integrates with Qt's signal-slot mechanism.
 * It allows logging messages from any thread and emits a signal to pass the message
 * to the GUI thread for display.
 */
class QtLogSink : public QObject, public enkas::logging::LogSink {
    Q_OBJECT

public:
    explicit QtLogSink(QObject* parent = nullptr) : QObject(parent) {}
    ~QtLogSink() override = default;

    /**
     * @brief Logs a message with the specified log level.
     * This method is called by the logging framework to log messages.
     * It emits a signal that can be connected to a slot for displaying the message.
     *
     * @param level The log level of the message.
     * @param message The message text to log.
     */
    void log(enkas::logging::LogLevel level, std::string_view message) override {
        emit messageLogged(level, QString::fromStdString(std::string(message)));
    }

signals:
    /** @signal
     * @brief Signal emitted when a log message is received.
     * This signal carries the log level and the message text.
     */
    void messageLogged(enkas::logging::LogLevel level, const QString& message);
};
