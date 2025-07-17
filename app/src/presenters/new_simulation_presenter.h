#pragma once

#include <enkas/data/system.h>

#include <QObject>
#include <optional>

#include "core/settings/settings.h"

class QTimer;
class QThread;
class INewSimulationView;
class FileParseWorker;
class SimulationManager;

class NewSimulationPresenter : public QObject {
    Q_OBJECT
public:
    explicit NewSimulationPresenter(INewSimulationView* view, QObject* parent = nullptr);
    ~NewSimulationPresenter();

public slots:
    void checkInitialSystemFile();
    void checkSettingsFile();
    void startSimulation();
    void abortSimulation();
    void openSimulationWindow();

private slots:
    void updatePreview();
    void updateProgress();
    void onSettingsParsed(const std::optional<Settings>& settings);
    void onInitialSystemParsed(const std::optional<enkas::data::System>& system);
    void onInitializationCompleted();

private:
    INewSimulationView* view_ = nullptr;

    QTimer* preview_timer_ = nullptr;
    QTimer* progress_timer_ = nullptr;

    FileParseWorker* file_parse_worker_ = nullptr;
    QThread* file_parse_thread_ = nullptr;

    SimulationManager* simulation_manager_ = nullptr;
};
