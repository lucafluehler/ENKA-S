#pragma once

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/simulation/simulator.h>

#include <QObject>
#include <memory>

#include "core/settings/settings.h"
#include "core/snapshot.h"

class SimulationWorker : public QObject {
    Q_OBJECT
public:
    SimulationWorker(const Settings& settings, QObject* parent = nullptr);
    ~SimulationWorker();

public slots:
    void startGeneration();
    void startInitialization();
    void step();

signals:
    void generationCompleted();
    void initializationCompleted();
    void simulationStep(double time,
                        SystemSnapshotPtr system_snapshot,
                        DiagnosticsSnapshotPtr diagnostics_snapshot);
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
