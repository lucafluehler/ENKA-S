// file_parse_worker.cpp
#include "file_parse_worker.h"

#include <filesystem>
#include <memory>
#include <optional>

#include "core/files/file_parse_logic.h"
#include "core/files/system_snapshot_stream.h"

void FileParseWorker::openSystemFile(const QString& file_path) {
    snapshot_provider_ = std::make_unique<SystemSnapshotStream>(file_path.toStdString());
    if (snapshot_provider_->initialize()) {
        emit systemFileOpened(snapshot_provider_->getAllTimestamps());
    } else {
        snapshot_provider_.reset();  // Release the failed provider
        emit systemFileOpened(std::nullopt);
    }
}

void FileParseWorker::requestSnapshotAt(double timestamp) {
    if (!snapshot_provider_) return;
    auto snapshot = snapshot_provider_->getSnapshotAt(timestamp);
    emit snapshotReady(snapshot);
}

void FileParseWorker::requestNextSnapshot() {
    if (!snapshot_provider_) return;
    auto snapshot = snapshot_provider_->getNextSnapshot();
    emit snapshotReady(snapshot);
}

void FileParseWorker::requestPreviousSnapshot() {
    if (!snapshot_provider_) return;
    auto snapshot = snapshot_provider_->getPreviousSnapshot();
    emit snapshotReady(snapshot);
}

void FileParseWorker::requestInitialSnapshot() {
    if (!snapshot_provider_) return;
    auto snapshot = snapshot_provider_->getFirstSnapshot();
    emit snapshotReady(snapshot);
}

void FileParseWorker::parseSettings(const QString& file_path) {
    auto settings = FileParseLogic::parseSettings(file_path.toStdString());
    emit settingsParsed(settings);
}

void FileParseWorker::parseDiagnosticsSeries(const QString& file_path) {
    auto series = FileParseLogic::parseDiagnosticsSeries(file_path.toStdString());
    emit diagnosticsSeriesParsed(series);
}
