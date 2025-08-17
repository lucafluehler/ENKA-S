#include "simulation_worker.h"

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/logging/logger.h>
#include <enkas/simulation/simulator.h>

#include <memory>

#include "core/dataflow/snapshot.h"
#include "core/factories/generator_factory.h"
#include "core/factories/simulator_factory.h"
#include "core/settings/settings.h"
#include "services/file_parser/file_parser.h"

SimulationWorker::SimulationWorker(const Settings& settings,
                                   std::shared_ptr<MemoryPools> memory_pools,
                                   std::shared_ptr<SimulationOutputs> outputs,
                                   std::shared_ptr<LiveDebugInfo> debug_info,
                                   QObject* parent)
    : QObject(parent),
      memory_pools_(memory_pools),
      outputs_(outputs),
      debug_info_(debug_info),
      system_step_(settings.get<double>(SettingKey::SystemDataStep)),
      diagnostics_step_(settings.get<double>(SettingKey::DiagnosticsDataStep)),
      duration_(settings.get<double>(SettingKey::Duration)) {
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

    // Setup memory pools for diagnostics data and snapshots. The system data pool will be
    // initialized later with the particle count from the initial system.
    memory_pools_->diagnostics_data_pool =
        std::make_shared<MemoryPool<enkas::data::Diagnostics>>(pool_size_);
    memory_pools_->system_snapshot_pool =
        std::make_shared<MemoryPool<Snapshot<enkas::data::System>>>(pool_size_);
    memory_pools_->diagnostics_snapshot_pool =
        std::make_shared<MemoryPool<Snapshot<enkas::data::Diagnostics>>>(pool_size_);

    // Update debug info
    debug_info_->system_data_pool_capacity = pool_size_;
    debug_info_->diagnostics_data_pool_capacity = pool_size_;
    debug_info_->system_snapshot_pool_capacity = pool_size_;
    debug_info_->diagnostics_snapshot_pool_capacity = pool_size_;

    ENKAS_LOG_INFO("Simulation worker initialized successfully.");
}

void SimulationWorker::startGeneration() {
    if (file_mode_) {
        // Load the initial system from a file
        auto initial_system_opt = FileParser().parseInitialSystem(file_path_);

        if (!initial_system_opt) {
            ENKAS_LOG_ERROR("Failed to parse initial system from file: {}", file_path_.string());
            return;
        }

        initial_system_ = std::make_unique<enkas::data::System>(std::move(*initial_system_opt));
    } else {
        // Generate the initial system using the generator
        if (!generator_) {
            ENKAS_LOG_ERROR("Generator is not initialized.");
            return;
        }

        initial_system_ = std::make_unique<enkas::data::System>(generator_->createSystem());
    }

    // Initialize the system data pool with the particle count from the initial system
    const size_t particle_count = initial_system_->count();
    memory_pools_->system_data_pool =
        std::make_shared<MemoryPool<enkas::data::System, size_t>>(pool_size_, particle_count);

    ENKAS_LOG_INFO("Initial system generated successfully with {} particles.", particle_count);
    emit generationCompleted();
}

void SimulationWorker::startInitialization() {
    if (!simulator_ || !initial_system_) {
        ENKAS_LOG_ERROR("Simulator or initial system is not initialized.");
        return;
    }

    // Pass two initial data buffers to the simulator
    auto initial_system_data = memory_pools_->system_data_pool->acquire();
    *initial_system_data = *initial_system_;

    auto temp_system_buffer = memory_pools_->system_data_pool->acquire();

    simulator_->initialize(initial_system_data, temp_system_buffer);

    ENKAS_LOG_INFO("Simulator successfully initialized with the initial system.");
    emit initializationCompleted();
}

void SimulationWorker::runSimulation() {
    if (!simulator_) {
        ENKAS_LOG_ERROR("Simulator is not initialized, cannot run simulation.");
        return;
    }

    ENKAS_LOG_INFO("Starting simulation...");

    // Reset time and step count
    double time = 0.0;
    time_.store(time);
    debug_info_->current_step.store(0, std::memory_order_relaxed);
    while (time < duration_ && !stop_requested_.load()) {
        const bool retrieve_system_data = (time - last_system_update_ >= system_step_);
        const bool retrieve_diagnostics_data =
            (time - last_diagnostics_update_ >= diagnostics_step_);

        std::shared_ptr<enkas::data::System> system_data = nullptr;
        std::shared_ptr<enkas::data::Diagnostics> diagnostics_data = nullptr;

        if (retrieve_system_data) {
            system_data = memory_pools_->system_data_pool->acquire();
        }

        if (retrieve_diagnostics_data) {
            diagnostics_data = memory_pools_->diagnostics_data_pool->acquire();
        }

        simulator_->step(system_data, diagnostics_data);
        time = simulator_->getSystemTime();
        time_.store(time);

        if (retrieve_system_data) {
            auto system_snapshot = memory_pools_->system_snapshot_pool->acquire();
            system_snapshot->data = std::move(system_data);
            system_snapshot->time = time;

            if (outputs_->rendering_snapshot) {
                outputs_->rendering_snapshot->store(system_snapshot, std::memory_order_release);
            }

            if (outputs_->system_storage_queue) {
                outputs_->system_storage_queue->pushBlocking(system_snapshot);
            }

            last_system_update_ = time;
        }

        if (retrieve_diagnostics_data) {
            auto diagnostics_snapshot = memory_pools_->diagnostics_snapshot_pool->acquire();
            diagnostics_snapshot->data = std::move(diagnostics_data);
            diagnostics_snapshot->time = time;

            if (outputs_->chart_queue) {
                outputs_->chart_queue->pushBlocking(diagnostics_snapshot);
            }

            if (outputs_->diagnostics_storage_queue) {
                outputs_->diagnostics_storage_queue->pushBlocking(diagnostics_snapshot);
            }

            last_diagnostics_update_ = time;
        }

        // Update debug info
        debug_info_->current_step.fetch_add(1, std::memory_order_relaxed);
    }

    ENKAS_LOG_INFO("Simulation completed successfully.");
}
