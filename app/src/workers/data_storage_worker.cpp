#include "data_storage_worker.h"

#include <QObject>

#include "../core/data_ptr.h"
#include "../core/data_storage_logic.h"

DataStorageWorker::DataStorageWorker(const std::shared_ptr<DataPtr>& data,
                                     const std::filesystem::path& output_dir,
                                     QObject* parent)
    : QObject(parent), data_(data), output_dir_(output_dir) {}

void DataStorageWorker::saveSettings() {
    DataStorageLogic::saveSettings(output_dir_, *data_->settings);
    emit workFinished();
}

void DataStorageWorker::saveSystemData(double time) {
    DataStorageLogic::saveSystemData(output_dir_, time, *data_->system);
    emit workFinished();
}

void DataStorageWorker::saveDiagnosticsData(double time) {
    DataStorageLogic::saveDiagnosticsData(output_dir_, time, *data_->diagnostics);
    emit workFinished();
}
