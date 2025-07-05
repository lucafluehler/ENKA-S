#pragma once

#include <QObject>
#include <QThread>

#include "settings.h"
#include "simulation_worker.h"
#include "data_saver.h"
#include "simulation_window.h"
#include "data_ptr.h"

class SimulationManager : public QObject
{
    Q_OBJECT
public:
    SimulationManager( const Settings& settings
                     , const DataSettings& data_settings
                     , const GenerationSettings& generation_settings
                     , const SimulationSettings& simulation_settings
                     , QObject* parent = nullptr);
    ~SimulationManager();

    void startSimulationProcedere();
    void abortSimulation();

    double getTime() const;
    double getDuration() const;

signals:
    void saveSettings();
    void saveInitialSystem();

    void requestGeneration();
    void requestInitialization();
    void requestSimulationStep();

    void generationCompleted();
    void initializationCompleted();
    void simulationStep(double time);

    void renderDataStep();
    void diagnosticsDataStep();
    void analyticsDataStep();

    void saveRenderData();
    void saveDiagnosticsData();
    void saveAnalyticsData();

public slots:
    void openSimulationWindow();

private slots:
    void dataSaverFinished();

    void receivedGenerationCompleted();
    void receivedInitializationCompleted();
    void receivedSimulationStep(double time);

    void threadFinished();

private:
    void performSimulationStep(double time);
    void logTime(const QString& time_identifier);

    DataSettings data_settings;
    double last_render_update;
    double last_diagnostics_update;
    double last_analytics_update;

    std::shared_ptr<Simulator> simulator;
    std::shared_ptr<Generator> generator;

    double duration;

    SimulationWindow *simulation_window;

    SimulationWorker *simulation_worker;
    QThread *simulation_thread;
    QElapsedTimer *simulation_timer;

    DataSaver *data_saver;
    QThread *data_saver_thread;
    int data_saver_workload;
    double pending_time;

    std::shared_ptr<DataPtr> data_ptr;

    bool aborted;
};
