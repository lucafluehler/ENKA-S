#ifndef SIMULATION_WORKER_H_
#define SIMULATION_WORKER_H_

#include <QObject>
#include <memory>

#include "utils.h"
#include "simulator.h"
#include "generator.h"

class SimulationWorker : public QObject
{
    Q_OBJECT
public:
    SimulationWorker( std::shared_ptr<Generator> generator
                    , std::shared_ptr<Simulator> simulator
                    , QObject* parent = nullptr );
    ~SimulationWorker();

    utils::InitialSystem getInitialSystem() const;

public slots:
    void startGeneration();
    void startInitialization();
    void evolveSystem();

signals:
    void generationCompleted();
    void initializationCompleted();
    void simulationStep(double time);
    void error();

private:
    std::shared_ptr<Generator> generator;
    std::shared_ptr<Simulator> simulator;

    utils::InitialSystem initial_system;
};

#endif // SIMULATION_WORKER_H_
