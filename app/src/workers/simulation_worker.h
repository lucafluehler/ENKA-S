#pragma once

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/simulation/simulator.h>

#include <QObject>
#include <atomic>
#include <cstddef>
#include <filesystem>
#include <memory>

#include "core/dataflow/blocking_queue.h"
#include "core/dataflow/debug_info.h"
#include "core/dataflow/latest_value_slot.h"
#include "core/dataflow/memory_pool.h"
#include "core/dataflow/snapshot.h"
#include "core/settings/settings.h"

/**
 * @brief Contains memory pools which contain pre-allocated memory for system and diagnostics data,
 * as well as snapshots of these data types. This is used to avoid frequent memory allocations and
 * deep copies during the simulation process, which can be expensive in terms of performance.
 */
struct MemoryPools {
    std::shared_ptr<MemoryPool<enkas::data::System, size_t>> system_data_pool = nullptr;
    std::shared_ptr<MemoryPool<enkas::data::Diagnostics>> diagnostics_data_pool = nullptr;
    std::shared_ptr<MemoryPool<Snapshot<enkas::data::System>>> system_snapshot_pool = nullptr;
    std::shared_ptr<MemoryPool<Snapshot<enkas::data::Diagnostics>>> diagnostics_snapshot_pool =
        nullptr;
};

/**
 * @brief Contains shared pointers which hold snapshot pointers to generated data. This is used to
 * share data between the producer and the consumers.
 */
struct SimulationOutputs {
    std::shared_ptr<LatestValueSlot<SystemSnapshot>> rendering_snapshot = nullptr;
    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> chart_queue = nullptr;
    std::shared_ptr<BlockingQueue<SystemSnapshotPtr>> system_storage_queue = nullptr;
    std::shared_ptr<BlockingQueue<DiagnosticsSnapshotPtr>> diagnostics_storage_queue = nullptr;
};

/**
 * @brief The SimulationWorker class is responsible for managing the simulation process,
 * including system generation, initialization, and stepping through the simulation. It is designed
 * to run in a separate thread.
 */
class SimulationWorker : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs a SimulationWorker with the given settings.
     * @param settings The settings to configure the worker.
     * @param memory_pools Shared pointers to memory pools for pre-allocated data.
     * @param outputs Shared pointers to output queues for simulation data.
     * @param debug_info A shared pointer to the debug information for live mode.
     * @param parent The parent QObject.
     */
    explicit SimulationWorker(const Settings& settings,
                              std::shared_ptr<MemoryPools> memory_pools,
                              std::shared_ptr<SimulationOutputs> outputs,
                              std::shared_ptr<LiveDebugInfo> debug_info,
                              QObject* parent = nullptr);
    ~SimulationWorker() override = default;

    /**
     * @brief Stops the simulation.
     */
    void abort() { stop_requested_.store(true); }

    /**
     * @brief Returns the current simulation time.
     * @return The current time in time step units.
     */
    double getTime() const { return time_.load(); }

    /**
     * @brief Starts the generation of the initial system.
     * If the generation method is set to File, it will load the system from a file.
     * Otherwise, it will generate the system using the configured generator.
     */
    void startGeneration();

    /**
     * @brief Starts the initialization of the simulator with the previously generated initial
     * system.
     */
    void startInitialization();

    /**
     * @brief Continuously steps through the simulation until the total duration is reached.
     */
    void runSimulation();

signals:
    /**
     * @brief Signals that the generation of the initial system has been completed.
     */
    void generationCompleted();

    /**
     * @brief Signals that the initialization of the simulator has been completed.
     */
    void initializationCompleted();

private:
    std::unique_ptr<enkas::generation::Generator> generator_;
    std::unique_ptr<enkas::simulation::Simulator> simulator_;

    std::shared_ptr<LiveDebugInfo> debug_info_;

    const size_t pool_size_ = 512;  // Default size for memory pools
    std::shared_ptr<MemoryPools> memory_pools_;
    std::shared_ptr<SimulationOutputs> outputs_;

    std::unique_ptr<enkas::data::System> initial_system_;

    bool file_mode_;                   // Indicates if the initial system is loaded from a file
    std::filesystem::path file_path_;  // Path to the file containing the initial system

    double last_system_update_ = 0.0;       // Last system time when the system data was retrieved
    double last_diagnostics_update_ = 0.0;  // Last system time when the diagnostics data was
                                            // retrieved

    double system_step_;       // After each system step, the system data will be retrieved
    double diagnostics_step_;  // After each diagnostics step, the diagnostics data will be

    double duration_;                 // Total duration of the simulation
    std::atomic<double> time_ = 0.0;  // Current simulation time

    std::atomic<bool> stop_requested_ = false;
};
