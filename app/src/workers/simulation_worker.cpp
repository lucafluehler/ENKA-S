#include "simulation_worker.h"

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/logging/logger.h>
#include <enkas/simulation/simulator.h>

#include <memory>

#include "core/files/file_parse_logic.h"
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

    ENKAS_LOG_INFO("Simulation worker initialized successfully.");
}

void SimulationWorker::startGeneration() {
    if (file_mode_) {
        // Load the initial system from a file
        auto initial_system_opt = FileParseLogic::parseInitialSystem(file_path_);

        if (!initial_system_opt) {
            ENKAS_LOG_ERROR("Failed to parse initial system from file: {}", file_path_.string());
            emit error();
            return;
        }

        initial_system_ = std::make_unique<enkas::data::System>(std::move(*initial_system_opt));
    } else {
        // Generate the initial system using the generator
        if (!generator_) {
            ENKAS_LOG_ERROR("Generator is not initialized.");
            emit error();
            return;
        }

        initial_system_ = std::make_unique<enkas::data::System>(generator_->createSystem());
    }

    ENKAS_LOG_INFO("Initial system generated successfully with {} particles.",
                   initial_system_->count());
    emit generationCompleted();
}

void SimulationWorker::startInitialization() {
    if (!simulator_ || !initial_system_) {
        ENKAS_LOG_ERROR("Simulator or initial system is not initialized.");
        emit error();
        return;
    }

    simulator_->setSystem(*initial_system_);
    ENKAS_LOG_INFO("Simulator successfully initialized with the initial system.");
    emit initializationCompleted();
}

void SimulationWorker::step() {
    if (!simulator_) {
        ENKAS_LOG_ERROR("Simulator is not initialized, cannot perform simulation step.");
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
