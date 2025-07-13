#include "file_check_worker.h"

#include <QDir>
#include <QString>
#include <filesystem>

#include "../core/file_check_logic.h"

void FileCheckWorker::checkFiles(const QVector<FileType>& files, const QString& dir_path) {
    for (const auto& file : files) {
        std::filesystem::path path;
        bool result = false;

        switch (file) {
            case FileType::Settings:
                path = std::filesystem::path(dir_path.toStdString()) / "settings.csv";
                result = FileCheckLogic::checkSettingsFile(path);
                break;
            case FileType::System:
                path = std::filesystem::path(dir_path.toStdString()) / "system.csv";
                result = FileCheckLogic::checkSystemFile(path);
                break;
            case FileType::DiagnosticsData:
                path = std::filesystem::path(dir_path.toStdString()) / "diagnostics_data.csv";
                result = FileCheckLogic::checkDiagnosticsFile(path);
                break;
            default:
                continue;
        }

        auto qt_path = QString::fromStdString(path.string());
        emit fileChecked(file, qt_path, result);
    }
}