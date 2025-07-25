#pragma once

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/simulation/simulator.h>

#include <QObject>
#include <memory>

#include "core/dataflow/snapshot.h"
#include "core/settings/settings.h"


/**
 * @brief The SimulationWorker class is responsible for managing the simulation process,
 * including system generation, initialization, and stepping through the simulation.
 */
class SimulationWorker : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs a SimulationWorker with the given settings.
     * @param settings The settings to configure the worker.
     * @param parent The parent QObject.
     */
    explicit SimulationWorker(const Settings& settings, QObject* parent = nullptr);
    ~SimulationWorker() override = default;

public slots:
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
     * @brief Performs a simulation step.
     */
    void step();

signals:
    /**
     * @brief Signals that the generation of the initial system has been completed.
     */
    void generationCompleted();

    /**
     * @brief Signals that the initialization of the simulator has been completed.
     */
    void initializationCompleted();

    /**
     * @brief Signals that a simulation step has been completed.
     * @param time The current simulation time.
     * @param system_snapshot If it is time to retrieve the system data, this will contain the
     * snapshot of the system data after the step. Otherwise, it will be nullptr.
     * @param diagnostics_snapshot If it is time to retrieve the diagnostics data, this will contain
     * the snapshot of the diagnostics data after the step. Otherwise, it will be nullptr.
     */
    void simulationStep(double time,
                        SystemSnapshotPtr system_snapshot,
                        DiagnosticsSnapshotPtr diagnostics_snapshot);

    /**
     * @brief Signals an error during the simulation process.
     */
    void error();

private:
    std::unique_ptr<enkas::generation::Generator> generator_;
    std::unique_ptr<enkas::simulation::Simulator> simulator_;

    std::unique_ptr<enkas::data::System> initial_system_;

    bool file_mode_;                   // Indicates if the initial system is loaded from a file
    std::filesystem::path file_path_;  // Path to the file containing the initial system

    double last_system_update_;       // Last system time when the system data was retrieved
    double last_diagnostics_update_;  // Last system time when the diagnostics data was retrieved

    double system_step_;       // After each system step, the system data will be retrieved
    double diagnostics_step_;  // After each diagnostics step, the diagnostics data will be
};
