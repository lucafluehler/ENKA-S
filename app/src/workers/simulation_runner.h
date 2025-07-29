#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QThread>

#include "core/dataflow/debug_info.h"
#include "core/settings/settings.h"
#include "presenters/simulation_window_presenter.h"
#include "queue_storage_worker.h"
#include "views/simulation_window/simulation_window.h"
#include "workers/simulation_worker.h"

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
    double getTime() const { return simulation_worker_ ? simulation_worker_->getTime() : 0.0; }

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
     * @brief Requests the simulation to start.
     */
    void requestSimulationStart();

    /**
     * @brief Emitted when the generation of a new initial system is completed.
     */
    void generationCompleted();

    /**
     * @brief Emitted when the initialization of the simulator is completed.
     */
    void initializationCompleted();

public slots:
    /**
     * @brief Opens the simulation window.
     */
    void openSimulationWindow();

private slots:
    void receivedGenerationCompleted();
    void receivedInitializationCompleted();
    void updateDebugInfo();

private:
    void setupOutputDir();
    void setupSystemStorageWorker();
    void setupDiagnosticsStorageWorker();
    void setupSimulationWorker(const Settings& settings);
    void setupSimulationWindow();

    double duration_;             // Total duration of the simulation
    bool save_system_data_;       // Flag to indicate if system data should be saved
    bool save_diagnostics_data_;  // Flag to indicate if diagnostics data should be saved

    std::filesystem::path output_dir_;  // Output directory for saving data

    SimulationWindow* simulation_window_;
    SimulationWindowPresenter* simulation_window_presenter_;

    SimulationWorker* simulation_worker_ = nullptr;
    QThread* simulation_thread_ = nullptr;

    QueueStorageWorkerBase* system_storage_worker_ = nullptr;
    QThread* system_storage_thread_ = nullptr;

    QueueStorageWorkerBase* diagnostics_storage_worker_ = nullptr;
    QThread* diagnostics_storage_thread_ = nullptr;

    std::shared_ptr<MemoryPools> memory_pools_;

    const size_t pool_size_ = 512;  // Default size for memory pools
    std::shared_ptr<SimulationOutputs> outputs_;

    std::shared_ptr<LiveDebugInfo> debug_info_;

    QTimer* debug_info_timer_ = nullptr;  // Timer for updating debug info

    bool aborted_ = false;  // Flag to indicate if the simulation was aborted
};
