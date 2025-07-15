#include "simulation_worker.h"

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/simulation/simulator.h>

#include <memory>

SimulationWorker::SimulationWorker(std::shared_ptr<enkas::generation::Generator> generator,
                                   std::shared_ptr<enkas::simulation::Simulator> simulator,
                                   QObject* parent)
    : QObject(parent), generator_(generator), simulator_(simulator) {}

SimulationWorker::~SimulationWorker() {
    generator_ = nullptr;
    simulator_ = nullptr;
}

enkas::data::System SimulationWorker::getInitialSystem() const { return initial_system_; }

void SimulationWorker::startGeneration() {
    if (!generator_) {
        emit error();
        return;
    }

    initial_system_ = generator_->createSystem();
    emit generationCompleted();
}

void SimulationWorker::startInitialization() {
    if (!simulator_) {
        emit error();
        return;
    }

    simulator_->setSystem(initial_system_);
    emit initializationCompleted();
}

void SimulationWorker::evolveSystem() {
    if (!simulator_) {
        emit error();
        return;
    }

    simulator_->step();
    emit simulationStep(simulator_->getSystemTime());
}
