#pragma once

#include <enkas/logging/logger.h>

#include <QObject>
#include <QString>

// Forward declaration to avoid including the whole header
namespace enkas::logging {
enum class LogLevel;
}

class QtLogSink : public QObject, public enkas::logging::LogSink {
    Q_OBJECT

public:
    explicit QtLogSink(QObject* parent = nullptr);

    // This method will be called from any thread by the logger
    void log(enkas::logging::LogLevel level, std::string_view message) override;

signals:
    // This signal will be emitted to safely pass the log message to the GUI thread
    void messageLogged(enkas::logging::LogLevel level, const QString& message);
};
