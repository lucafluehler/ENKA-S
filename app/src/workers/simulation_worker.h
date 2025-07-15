#pragma once

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>
#include <enkas/simulation/simulator.h>

#include <QObject>
#include <memory>

class SimulationWorker : public QObject {
    Q_OBJECT
public:
    SimulationWorker(std::shared_ptr<enkas::generation::Generator> generator,
                     std::shared_ptr<enkas::simulation::Simulator> simulator,
                     QObject* parent = nullptr);
    ~SimulationWorker();

    enkas::data::System getInitialSystem() const;

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
    std::shared_ptr<enkas::generation::Generator> generator_;
    std::shared_ptr<enkas::simulation::Simulator> simulator_;

    enkas::data::System initial_system_;
};
