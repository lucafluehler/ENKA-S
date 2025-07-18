#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QThread>

#include "core/blocking_queue.h"
#include "core/settings/settings.h"
#include "core/snapshot.h"
#include "presenters/simulation_window_presenter.h"
#include "queue_storage_worker.h"
#include "simulation_worker.h"
#include "views/simulation_window/simulation_window.h"

class SimulationManager : public QObject {
    Q_OBJECT
public:
    SimulationManager(const Settings& settings, QObject* parent = nullptr);
    ~SimulationManager();

    void startSimulationProcedere();

    double getDuration() const;
    double getTime() const;

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

    void saveRenderData();
    void saveDiagnosticsData();

public slots:
    void openSimulationWindow();

private slots:
    void dataSaverFinished();

    void receivedGenerationCompleted();
    void receivedInitializationCompleted();
    void receivedSimulationStep(double time,
                                SystemSnapshotPtr system_snapshot,
                                DiagnosticsSnapshotPtr diagnostics_snapshot);

private:
    void performSimulationStep(double time,
                               SystemSnapshotPtr system_snapshot,
                               DiagnosticsSnapshotPtr diagnostics_snapshot);
    void saveSettingsToFile();
    void setupOutputDir();
    void setupSystemStorageWorker();
    void setupDiagnosticsStorageWorker();
    void setupSimulationWorker(const Settings& settings);
    void setupSimulationWindow();

    double duration_;             // Total duration of the simulation
    double time_;                 // Current simulation time
    bool save_system_data_;       // Flag to indicate if system data should be saved
    bool save_diagnostics_data_;  // Flag to indicate if diagnostics data should be saved

    std::filesystem::path output_dir_;  // Output directory for saving data

    SimulationWindow* simulation_window_;
    SimulationWindowPresenter* simulation_window_presenter_;

    SimulationWorker* simulation_worker_;
    QThread* simulation_thread_;

    QueueStorageWorkerBase* system_storage_worker_;
    QThread* system_storage_thread_;

    QueueStorageWorkerBase* diagnostics_storage_worker_;
    QThread* diagnostics_storage_thread_;

    std::atomic<SystemSnapshotPtr> render_queue_slot_;
    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue_;
    std::shared_ptr<BlockingQueue<SystemSnapshotPtr>> system_storage_queue_;
    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> diagnostics_storage_queue_;

    bool aborted_;  // Flag to indicate if the simulation was aborted
};
