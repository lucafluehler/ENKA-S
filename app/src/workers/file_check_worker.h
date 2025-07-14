#pragma once

#include <QObject>

#include "../core/file_types.h"

class FileCheckWorker : public QObject {
    Q_OBJECT

public:
    explicit FileCheckWorker(QObject* parent = nullptr);

public slots:
    void checkFiles(const QVector<FileType>& files, const QString& dir_path);

signals:
    void fileChecked(const FileType& file, const QString& path, bool result);
};
