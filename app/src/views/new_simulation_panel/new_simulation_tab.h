#pragma once

#include <QWidget>
#include <QString>
#include <QMap>
#include <QTimer>
#include <QVariant>
#include <memory>

#include "settings.h"
#include "simulation_manager.h"
#include "file_checker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class NewSimulationTab; }
QT_END_NAMESPACE

class NewSimulationTab : public QWidget
{
    Q_OBJECT

public:
    NewSimulationTab(QWidget *parent = nullptr);
    ~NewSimulationTab();

private slots:
    void resetSettings();
    void checkSaveFolder(int state);
    void randomizeSeed();

    void openSystemDataDialog();
    void openSettingsDialog();
    void onFileChecked(FileType file, const QString& path, bool result);

    void createSimulationManager();
    void showSimulationProgress();

    void abortSimulation();
    void simulationAborted();

    void updateSimulationProgress();

    void updatePreview();

private:
    void setupInitialSystemPreviews();
    void loadSystemData(const QString& folder_name);
    void mapStringsToUiPtrs();
    Settings parseSettingsCSV(const QString& path, bool* is_valid = nullptr) const;
    Settings fetchSettings(bool* is_valid = nullptr) const;

    Settings default_settings;
    QMap<QString, QObject*> setting_ptrs;

    SimulationManager *simulation_manager;

    FileChecker *file_checker;
    QString initial_system_path;

    QTimer *preview_timer;
    QTimer *progress_timer;
    Ui::NewSimulationTab *ui;
};
