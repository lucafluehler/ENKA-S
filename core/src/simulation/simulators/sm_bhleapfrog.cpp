#include "sm_bhleapfrog.h"
#include "utils.h"

SM_BHLeapfrog::SM_BHLeapfrog(const Settings& settings)
    : settings(settings)
    , system(std::make_shared<System>())
    , global_time(0.0)
    , c_SOFT_SQR(std::pow(settings.soft_param, 2))
{}

SM_BHLeapfrog::~SM_BHLeapfrog()
{
    system = nullptr;
}

//------------------------------------------------------------------------------------------

void SM_BHLeapfrog::initializeSystem(const utils::InitialSystem& initial_system)
{
    // Calculate kinetic and potential energies of the initial system
    system->reserve(initial_system.size());
    for (const auto& initial_particle : initial_system) {
        system->push_back(BHParticle(initial_particle));
    }

    // Calculate total energy
    bh_tree.build(system);
    double e_pot = bh_tree.getTotalEPot(settings.MAC, settings.soft_param);
    double e_kin = utils::getKineticEnergy(*system);
    const double c_ABS_E_TOT = std::abs(e_kin + e_pot*utils::G);

    // Scale particles to Hénon units
    utils::scaleParticles(*system, c_ABS_E_TOT);

    global_time = 0.0;
}

void SM_BHLeapfrog::evolveSystem()
{
    if (is_abortion_requested) return;

    for (auto& particle : *system){
        particle.vel += particle.acc*settings.time_step*0.5;
        particle.pos += particle.vel*settings.time_step;
    }

    bh_tree.updateAcclerations(system, settings.MAC, settings.soft_param);

    for (auto& particle : *system){
        particle.vel += particle.acc*settings.time_step*0.5;
    }

    global_time += settings.time_step;
}

double SM_BHLeapfrog::getGlobalTime() const { return global_time; }

RenderData SM_BHLeapfrog::getRenderData() const
{
    return utils::getRenderData(global_time, *system);
}

DiagnosticsData SM_BHLeapfrog::getDiagnosticsData() const
{
    const double c_E_POT = bh_tree.getTotalEPot(settings.MAC, settings.soft_param);

    return utils::getDiagnosticsData(global_time, c_E_POT, *system);
}

AnalyticsData SM_BHLeapfrog::getAnalyticsData() const
{
    return utils::getAnalyticsData(global_time, *system);
}
