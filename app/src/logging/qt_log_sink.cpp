#include "qt_log_sink.h"

QtLogSink::QtLogSink(QObject* parent) : QObject(parent) {}

void QtLogSink::log(enkas::logging::LogLevel level, std::string_view message) {
    emit messageLogged(level, QString::fromStdString(std::string(message)));
}
