#include "file_parse_worker.h"

#include <QObject>

#include "core/file_parse_logic.h"

FileParseWorker::FileParseWorker(QObject* parent) : QObject(parent) {}

void FileParseWorker::parseSettings(const QString& file_path) {
    auto settings = FileParseLogic::parseSettings(file_path.toStdString());
    emit settingsParsed(settings, file_path);
}

void FileParseWorker::parseNextSystemFrame(const QString& file_path, double previous_timestamp) {
    auto frame = FileParseLogic::parseNextSystemFrame(file_path.toStdString(), previous_timestamp);
    emit systemFrameParsed(frame, file_path);
}

void FileParseWorker::parseInitialSystem(const QString& file_path) {
    auto system = FileParseLogic::parseInitialSystem(file_path.toStdString());
    emit initialSystemParsed(system, file_path);
}

void FileParseWorker::parseSystemTimestamps(const QString& file_path) {
    auto timestamps = FileParseLogic::parseSystemTimestamps(file_path.toStdString());
    emit systemTimestampsParsed(timestamps, file_path);
}

void FileParseWorker::parseDiagnosticsSeries(const QString& file_path) {
    auto series = FileParseLogic::parseDiagnosticsSeries(file_path.toStdString());
    emit diagnosticsSeriesParsed(series, file_path);
}
