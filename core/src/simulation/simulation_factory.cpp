#include "simulation_factory.h"
#include "simulation_settings.h"
#include "simulator.h"

#include "sm_euler.h"
#include "sm_leapfrog.h"
#include "sm_hermite.h"
#include "sm_hits.h"
#include "sm_hacs.h"
#include "sm_bhleapfrog.h"

std::shared_ptr<Simulator> SimulationFactory::create(const SimulationSettings& settings)
{
    if (!settings.isValid()) return nullptr;

    switch (settings.method)
    {
        case SimulationMethod::Euler:
            return std::make_shared<SM_Euler>(settings.euler_settings);
            break;

        case SimulationMethod::Leapfrog:
            return std::make_shared<SM_Leapfrog>(settings.leapfrog_settings);
            break;

        case SimulationMethod::Hermite:
            return std::make_shared<SM_Hermite>(settings.hermite_settings);
            break;

        case SimulationMethod::HITS:
            return std::make_shared<SM_HITS>(settings.hits_settings);
            break;

        case SimulationMethod::HACS:
            return std::make_shared<SM_HACS>(settings.hacs_settings);;
            break;

        case SimulationMethod::BHLeapfrog:
            return std::make_shared<SM_BHLeapfrog>(settings.bhleapfrog_settings);
            break;

        default:
            break;
    };

    return nullptr;
}
