#pragma once

#include <QObject>
#include <filesystem>

#include "../core/data_ptr.h"

class DataStorageWorker : public QObject {
    Q_OBJECT

public:
    explicit DataStorageWorker(const std::shared_ptr<DataPtr>& data,
                               const std::filesystem::path& output_dir,
                               QObject* parent = nullptr);

signals:
    void workFinished();

public slots:
    void saveSettings();
    void saveSystemData(double time);
    void saveDiagnosticsData(double time);

private:
    std::shared_ptr<DataPtr> data_;
    std::filesystem::path output_dir_;
};
