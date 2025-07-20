#include "file_parse_worker.h"

#include <QObject>

#include "core/file_parse_logic.h"

FileParseWorker::FileParseWorker(QObject* parent) : QObject(parent) {}

void FileParseWorker::parseSettings(const QString& file_path) {
    auto settings = FileParseLogic::parseSettings(file_path.toStdString());
    emit settingsParsed(settings);
}

void FileParseWorker::parseNextSystemSnapshot(const QString& file_path, double previous_timestamp) {
    auto snapshot =
        FileParseLogic::parseNextSystemSnapshot(file_path.toStdString(), previous_timestamp);
    emit systemSnapshotParsed(snapshot);
}

void FileParseWorker::parseInitialSystem(const QString& file_path) {
    auto system = FileParseLogic::parseInitialSystem(file_path.toStdString());
    emit initialSystemParsed(system);
}

void FileParseWorker::parseSystemTimestamps(const QString& file_path) {
    auto timestamps = FileParseLogic::parseSystemTimestamps(file_path.toStdString());
    emit systemTimestampsParsed(timestamps);
}

void FileParseWorker::parseDiagnosticsSeries(const QString& file_path) {
    auto series = FileParseLogic::parseDiagnosticsSeries(file_path.toStdString());
    emit diagnosticsSeriesParsed(series);
}
