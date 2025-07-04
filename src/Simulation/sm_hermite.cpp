#include "sm_hermite.h"
#include "utils.h"

SM_Hermite::SM_Hermite(const Settings& settings)
    : settings(settings)
    , global_time(0.0)
    , e_pot(0.0)
    , c_SOFT_SQR(settings.soft_param*settings.soft_param)
{}

//------------------------------------------------------------------------------------------

void SM_Hermite::initializeSystem(const utils::InitialSystem& initial_system)
{
    system = System(initial_system.begin(), initial_system.end());

    // Calculate total energy
    calculateAccJrkEpot();
    const double c_ABS_E_TOT = std::abs(utils::getKineticEnergy(system) + e_pot*utils::G);

    // Scale particles to Hénon Units
    utils::scaleParticles(system, c_ABS_E_TOT);

    global_time = 0.0;
}

void SM_Hermite::evolveSystem()
{
    if (is_abortion_requested) return;

    System old_system = system;

    const double c_DT = settings.time_step;
    const double c_DT2 = c_DT*c_DT;
    const double c_DT3 = c_DT2*c_DT;

    // Predict particle position and velocity up to order jerk
    for (auto& particle : system) {
        particle.pos +=   particle.vel*c_DT
                        + particle.acc*c_DT2/2
                        + particle.jrk*c_DT3/6;

        particle.vel +=   particle.acc*c_DT
                        + particle.jrk*c_DT2/2;
    }

    // Calculate acceleration, jerk and potential energy for the entire system
    calculateAccJrkEpot();

    // Correct particle position and velocity using hermite scheme
    for (size_t i = 0; i < system.size(); i++) {
        auto& particle = system.at(i);
        const auto& old_particle = old_system.at(i);

        particle.vel =   old_particle.vel
                       + (old_particle.acc + particle.acc)*c_DT/2
                       + (old_particle.jrk - particle.jrk)*c_DT2/12;

        particle.pos =   old_particle.pos
                       + (old_particle.vel + particle.vel)*c_DT/2
                       + (old_particle.acc - particle.acc)*c_DT2/12;
    }

    // Update global time with time_step
    global_time += settings.time_step;
}

double SM_Hermite::getGlobalTime() const { return global_time; }

RenderData SM_Hermite::getRenderData() const
{
    return utils::getRenderData(global_time, system);
}

DiagnosticsData SM_Hermite::getDiagnosticsData() const
{
    return utils::getDiagnosticsData(global_time, e_pot, system);
}

AnalyticsData SM_Hermite::getAnalyticsData() const
{
    return utils::getAnalyticsData(global_time, system);
}

//------------------------------------------------------------------------------------------

void SM_Hermite::calculateAccJrkEpot()
{
    e_pot = 0.0;

    // Set all accelerations and jerks to 0.0
    for (auto& particle : system) {
        particle.acc.fill(0.0);
        particle.jrk.fill(0.0);
    }

    // Calculate pair-wise accelerations and epot simultaneously
    for (size_t i = 0; i < system.size(); i++) {
        if (is_abortion_requested) return;
        auto& particle_i = system.at(i);

        for (size_t j = i + 1; j < system.size(); j++) {
            auto& particle_j = system.at(j);

            // Acceleration
            const ga::Vector3D c_POS_JI = particle_j.pos - particle_i.pos;
            const double c_DIST_SQR = c_POS_JI.norm2() + c_SOFT_SQR;
            const double c_DIST     = std::sqrt(c_DIST_SQR);
            const double c_DIST_CUB = c_DIST*c_DIST_SQR;
            const ga::Vector3D c_ACC_TERM = c_POS_JI/c_DIST_CUB;

            particle_i.acc += c_ACC_TERM*particle_j.mass;
            particle_j.acc -= c_ACC_TERM*particle_i.mass;

            // Jerk
            const ga::Vector3D c_VEL_JI = particle_j.vel - particle_i.vel;
            const double c_RV = ga::dotProduct(c_POS_JI, c_VEL_JI)/c_DIST_SQR;
            const ga::Vector3D c_JRK_TERM = (c_VEL_JI - c_POS_JI*c_RV*3.0)/c_DIST_CUB;

            particle_i.jrk += c_JRK_TERM*particle_j.mass;
            particle_j.jrk -= c_JRK_TERM*particle_i.mass;

            // Potential Energy
            e_pot -= particle_i.mass*particle_j.mass/c_DIST;
        }
    }
}
