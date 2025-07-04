#include <memory>

#include "simulation_worker.h"
#include "simulator.h"
#include "generator.h"

SimulationWorker::SimulationWorker( std::shared_ptr<Generator> generator
                                  , std::shared_ptr<Simulator> simulator
                                  , QObject* parent )
    : QObject(parent)
    , generator(generator)
    , simulator(simulator)
{}

SimulationWorker::~SimulationWorker()
{
    generator = nullptr;
    simulator = nullptr;
}


utils::InitialSystem SimulationWorker::getInitialSystem() const
{
    return initial_system;
}


void SimulationWorker::startGeneration()
{
    if (!generator) {
        emit error();
        return;
    }

    initial_system = generator->createSystem();
    emit generationCompleted();
}

void SimulationWorker::startInitialization()
{
    if (!simulator) {
        emit error();
        return;
    }

    simulator->initializeSystem(initial_system);
    emit initializationCompleted();
}

void SimulationWorker::evolveSystem()
{
    if (!simulator) {
        emit error();
        return;
    }

    simulator->evolveSystem();
    emit simulationStep(simulator->getGlobalTime());
}
