#pragma once

#include <QObject>

#include "../core/file_types.h"

class QTimer;
class QThread;
class ILoadSimulationView;
class FileCheckWorker;

class LoadSimulationPresenter : public QObject {
    Q_OBJECT
public:
    explicit LoadSimulationPresenter(ILoadSimulationView* view, QObject* parent = nullptr);
    ~LoadSimulationPresenter();

    void isSelected(bool selected);

public slots:
    void checkFiles(const QVector<FileType>& files, const QString& file_path);

private slots:
    void onTimerTimeout();
    void onFileChecked(FileType file, const QString& path, bool result);

private:
    ILoadSimulationView* view_ = nullptr;
    QTimer* preview_timer_ = nullptr;
    FileCheckWorker* file_check_worker_ = nullptr;
    QThread* file_check_thread_ = nullptr;
};
