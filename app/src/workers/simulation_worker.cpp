#include "simulation_worker.h"

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/simulation/simulator.h>

#include <memory>

#include "core/file_parse_logic.h"
#include "core/generator_factory.h"
#include "core/settings/settings.h"
#include "core/simulator_factory.h"
#include "core/snapshot.h"

SimulationWorker::SimulationWorker(const Settings& settings, QObject* parent)
    : QObject(parent),
      last_system_update_(0.0),
      last_diagnostics_update_(0.0),
      system_step_(settings.get<double>(SettingKey::SystemDataStep)),
      diagnostics_step_(settings.get<double>(SettingKey::DiagnosticsDataStep)) {
    // Setup generator
    auto method = settings.get<GenerationMethod>(SettingKey::GenerationMethod);
    file_mode_ = (method == GenerationMethod::File);

    if (file_mode_) {
        generator_ = nullptr;  // We will generate the system later from a file
        file_path_ = settings.get<std::string>(SettingKey::FilePath);
    } else {
        generator_ = GeneratorFactory::create(settings);
    }

    // Setup simulator
    simulator_ = SimulatorFactory::create(settings);
}

SimulationWorker::~SimulationWorker() {
    generator_ = nullptr;
    simulator_ = nullptr;
}

void SimulationWorker::startGeneration() {
    if (file_mode_) {
        // Load the initial system from a file
        auto initial_system_opt = FileParseLogic::parseInitialSystem(file_path_);

        if (!initial_system_opt) {
            emit error();
            return;
        }

        initial_system_ = std::make_unique<enkas::data::System>(std::move(*initial_system_opt));
    } else {
        // Generate the initial system using the generator
        if (!generator_) {
            emit error();
            return;
        }

        initial_system_ = std::make_unique<enkas::data::System>(generator_->createSystem());
    }

    emit generationCompleted();
}

void SimulationWorker::startInitialization() {
    if (!simulator_ || !initial_system_) {
        emit error();
        return;
    }

    simulator_->setSystem(*initial_system_);
    emit initializationCompleted();
}

void SimulationWorker::step() {
    if (!simulator_) {
        emit error();
        return;
    }

    simulator_->step();

    // Check whether it is time to retrieve data
    double time = simulator_->getSystemTime();

    SystemSnapshotPtr system_snapshot{nullptr};
    DiagnosticsSnapshotPtr diagnostics_snapshot{nullptr};

    if (time - last_system_update_ >= system_step_) {
        system_snapshot = std::make_shared<SystemSnapshot>(simulator_->getSystem(), time);
        last_system_update_ = time;
    }

    if (time - last_diagnostics_update_ >= diagnostics_step_) {
        diagnostics_snapshot =
            std::make_shared<DiagnosticsSnapshot>(simulator_->getDiagnostics(), time);
        last_diagnostics_update_ = time;
    }

    emit simulationStep(time, system_snapshot, diagnostics_snapshot);
}
