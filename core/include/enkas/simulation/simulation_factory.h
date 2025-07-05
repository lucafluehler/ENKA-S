#ifndef SIMULATION_FACTORY_H_
#define SIMULATION_FACTORY_H_

#include <memory>

#include "simulation_settings.h"
#include "simulator.h"

class SimulationFactory {
public:
    static std::shared_ptr<Simulator> create(const SimulationSettings& settings);
};

#endif // SIMULATION_FACTORY_H_
