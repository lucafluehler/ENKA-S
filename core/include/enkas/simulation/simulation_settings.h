#ifndef SIMULATION_SETTINGS_H_
#define SIMULATION_SETTINGS_H_

#include "sm_euler.h"
#include "sm_leapfrog.h"
#include "sm_hermite.h"
#include "sm_hits.h"
#include "sm_hacs.h"
#include "sm_bhleapfrog.h"

enum class SimulationMethod
{
    Euler,
    Leapfrog,
    Hermite,
    HITS,
    HACS,
    BHLeapfrog
};

struct SimulationSettings
{
    SimulationMethod method;
    double duration;

    SM_Euler::Settings euler_settings;
    SM_Leapfrog::Settings leapfrog_settings;
    SM_Hermite::Settings hermite_settings;
    SM_HITS::Settings hits_settings;
    SM_HACS::Settings hacs_settings;
    SM_BHLeapfrog::Settings bhleapfrog_settings;

    bool isValid() const
    {
        switch (method) {
        case SimulationMethod::Euler: return euler_settings.isValid(); break;
        case SimulationMethod::Leapfrog: return leapfrog_settings.isValid(); break;
        case SimulationMethod::Hermite: return hermite_settings.isValid(); break;
        case SimulationMethod::HITS: return hits_settings.isValid(); break;
        case SimulationMethod::HACS: return hacs_settings.isValid(); break; break;
        case SimulationMethod::BHLeapfrog: return bhleapfrog_settings.isValid(); break;
        default: break;
        }

        return false;
    }
};

#endif // SIMULATION_SETTINGS_H_
