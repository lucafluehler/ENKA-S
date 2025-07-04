#include "sm_euler.h"
#include "utils.h"

SM_Euler::SM_Euler(const Settings& settings)
    : settings(settings)
    , global_time(0.0)
    , c_SOFT_SQR(settings.soft_param*settings.soft_param)
{}

//------------------------------------------------------------------------------------------

void SM_Euler::initializeSystem(const utils::InitialSystem& initial_particles)
{
    system = System(initial_particles.begin(), initial_particles.end());

    // Calculate total energy
    calculateAccEpot();
    const double c_ABS_E_TOT = std::abs(utils::getKineticEnergy(system) + e_pot*utils::G);

     // Scales particles to Hénon Units.
    utils::scaleParticles(system, c_ABS_E_TOT);

    global_time = 0.0;
}

void SM_Euler::evolveSystem()
{
    if (is_abortion_requested) return;

    // Calculate acceleration and potential energy for the entire system
    calculateAccEpot();

    // Calculate the new position and velocity of each particle using the
    // previously calculated acceleration
    for (auto& particle : system) {
        if (is_abortion_requested) return;

        particle.pos += particle.vel*settings.time_step;
        particle.vel += particle.acc*settings.time_step;
    }

    // Update global time with time_step
    global_time += settings.time_step;
}

double SM_Euler::getGlobalTime() const { return global_time; }

RenderData SM_Euler::getRenderData() const
{
    return utils::getRenderData(global_time, system);
}

DiagnosticsData SM_Euler::getDiagnosticsData() const
{
    return utils::getDiagnosticsData(global_time, e_pot, system);
}

AnalyticsData SM_Euler::getAnalyticsData() const
{
    return utils::getAnalyticsData(global_time, system);
}

//------------------------------------------------------------------------------------------

void SM_Euler::calculateAccEpot()
{
    e_pot = 0.0;

    // Set all accelerations to 0.0
    for (auto& particle : system) {
        particle.acc = ga::Vector3D();
    }

    // Calculate pair-wise accelerations and epot simultaneously
    for (size_t i = 0; i < system.size(); i++) {
        if (is_abortion_requested) return;
        auto& particle_i = system.at(i);

        for (size_t j = i + 1; j < system.size(); j++) {
            auto& particle_j = system.at(j);

            const ga::Vector3D c_POS_JI = particle_j.pos - particle_i.pos;
            const double c_DIST_SQR = c_POS_JI.norm2() + c_SOFT_SQR;
            const double c_DIST     = std::sqrt(c_DIST_SQR);
            const double c_DIST_CUB = c_DIST*c_DIST_SQR;

            particle_i.acc += c_POS_JI*particle_j.mass/c_DIST_CUB;
            particle_j.acc -= c_POS_JI*particle_i.mass/c_DIST_CUB;

            e_pot -= particle_i.mass*particle_j.mass/c_DIST;
        }
    }
}
