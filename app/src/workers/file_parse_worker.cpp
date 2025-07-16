#include "file_parse_worker.h"

#include <QObject>

#include "core/file_parse_logic.h"

FileParseWorker::FileParseWorker(QObject* parent) : QObject(parent) {}

void FileParseWorker::parseSettings(const QString& file_path) {
    auto settings = FileParseLogic::parseSettings(file_path.toStdString());

    if (settings) {
        emit settingsParsed(*settings);
    } else {
        emit parsingFailed("Failed to parse settings from file: " + file_path);
    }
}

void FileParseWorker::parseNextSystemFrame(const QString& file_path, double previous_timestamp) {
    auto frame = FileParseLogic::parseNextSystemFrame(file_path.toStdString(), previous_timestamp);

    if (frame) {
        emit systemFrameParsed(*frame);
    } else {
        emit parsingFailed("Failed to parse next system frame from file: " + file_path);
    }
}

void FileParseWorker::parseSystemTimestamps(const QString& file_path) {
    auto timestamps = FileParseLogic::parseSystemTimestamps(file_path.toStdString());

    if (timestamps) {
        emit systemTimestampsParsed(*timestamps);
    } else {
        emit parsingFailed("Failed to parse system timestamps from file: " + file_path);
    }
}

void FileParseWorker::parseDiagnosticsSeries(const QString& file_path) {
    auto series = FileParseLogic::parseDiagnosticsSeries(file_path.toStdString());

    if (series) {
        emit diagnosticsSeriesParsed(*series);
    } else {
        emit parsingFailed("Failed to parse diagnostics series from file: " + file_path);
    }
}
