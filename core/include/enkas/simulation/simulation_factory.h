#pragma once

#include <memory>

#include "simulation_settings.h"
#include "simulator.h"

class SimulationFactory {
public:
    static std::shared_ptr<Simulator> create(const SimulationSettings& settings);
};
