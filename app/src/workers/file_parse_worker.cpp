// file_parse_worker.cpp
#include "file_parse_worker.h"

#include <filesystem>

#include "core/files/file_parse_logic.h"

void FileParseWorker::parseSettings(const QString& file_path) {
    auto settings = FileParseLogic::parseSettings(file_path.toStdString());
    emit settingsParsed(settings);
}

void FileParseWorker::parseDiagnosticsSeries(const QString& file_path) {
    auto series = FileParseLogic::parseDiagnosticsSeries(file_path.toStdString());
    emit diagnosticsSeriesParsed(series);
}

void FileParseWorker::parseInitialSystem(const QString& file_path) {
    auto system = FileParseLogic::parseInitialSystem(file_path.toStdString());
    emit initialSystemParsed(system);
}

void FileParseWorker::countSnapshots(const QString& file_path) {
    auto count = FileParseLogic::countSnapshots(file_path.toStdString());
    emit snapshotsCounted(count);
}

void FileParseWorker::retrieveSimulationDuration(const QString& file_path) {
    auto duration = FileParseLogic::retrieveSimulationDuration(file_path.toStdString());
    emit simulationDurationRetrieved(duration);
}