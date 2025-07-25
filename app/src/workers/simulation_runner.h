#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QThread>

#include "core/dataflow/blocking_queue.h"
#include "core/dataflow/snapshot.h"
#include "core/settings/settings.h"
#include "presenters/simulation_window_presenter.h"
#include "queue_storage_worker.h"
#include "simulation_worker.h"
#include "views/simulation_window/simulation_window.h"

/**
 * @brief The SimulationRunner class manages the simulation process, including
 *        initialization, running the simulation, and handling data storage.
 */
class SimulationRunner : public QObject {
    Q_OBJECT
public:
    explicit SimulationRunner(const Settings& settings, QObject* parent = nullptr);
    ~SimulationRunner();

    /**
     * @brief Starts a new simulation.
     */
    void startSimulationProcedure() { emit requestGeneration(); }

    /**
     * @brief Returns the total duration of the simulation.
     * @return The duration in time step units.
     */
    double getDuration() const { return duration_; }

    /**
     * @brief Returns the current simulation time.
     * @return The current time in time step units.
     */
    double getTime() const { return time_; }

signals:
    /**
     * @brief Requests the generation of a new initial system.
     */
    void requestGeneration();

    /**
     * @brief Requests the initialization of the simulator with the previously generated initial
     * system.
     */
    void requestInitialization();

    /**
     * @brief Requests a simulation step.
     */
    void requestSimulationStep();

    /**
     * @brief Emitted when the generation of a new initial system is completed.
     */
    void generationCompleted();

    /**
     * @brief Emitted when the initialization of the simulator is completed.
     */
    void initializationCompleted();

    /**
     * @brief Emitted when a simulation step is completed.
     */
    void simulationStep(double time);

    /**
     * @brief Emitted when new diagnostics data is available.
     */
    void diagnosticsDataStep();

    /**
     * @brief Requests saving the latest render data.
     */
    void saveRenderData();

    /**
     * @brief Requests saving the latest diagnostics data.
     */
    void saveDiagnosticsData();

public slots:
    /**
     * @brief Opens the simulation window.
     */
    void openSimulationWindow();

private slots:
    void receivedGenerationCompleted();
    void receivedInitializationCompleted();
    void receivedSimulationStep(double time,
                                SystemSnapshotPtr system_snapshot,
                                DiagnosticsSnapshotPtr diagnostics_snapshot);

private:
    void performSimulationStep(double time,
                               SystemSnapshotPtr system_snapshot,
                               DiagnosticsSnapshotPtr diagnostics_snapshot);
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
