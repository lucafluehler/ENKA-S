#pragma once

#include <QWidget>
#include <QThread>
#include <QTimer>
#include "file_checker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoadSimulationTab; }
QT_END_NAMESPACE

struct FilePaths
{
    QString settings_path = "";
    QString initial_system_path = "";
    QString render_data_path = "";
    QString diagnostics_data_path = "";
    QString analytics_data_path = "";
};

class LoadSimulationTab : public QWidget
{
    Q_OBJECT

public:
    LoadSimulationTab(QWidget *parent = nullptr);
    ~LoadSimulationTab();

signals:
    void requestFilesCheck(const QVector<FileType> files, const QString& file_path);

public slots:
    void onTabSelected();
    void onTabClosed();

private slots:
    void updatePreview();

    void openFolderDialog();

    void onFileChecked(FileType file, const QString& path, bool result);
    void onFilesChecked();

    void openSettings();
    void openInitialSystem();
    void run();

private:
    void resetSimulationFilePaths();
    void loadSettings(const QString& file_path);
    void checkFiles(const QString& dir_path);

    FilePaths file_paths;

    Ui::LoadSimulationTab *ui;
    FileChecker *file_checker;
    QThread *file_checker_thread;

    QTimer *preview_timer;
};
