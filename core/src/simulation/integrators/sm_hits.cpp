#include "sm_hits.h"
#include "utils.h"

SM_HITS::SM_HITS(const Settings& settings)
    : settings(settings)
    , global_time(0.0)
    , c_SOFT_SQR(settings.soft_param*settings.soft_param)
{}

//------------------------------------------------------------------------------------------

void SM_HITS::initializeSystem(const utils::InitialSystem& initial_system)
{
    system = System(initial_system.begin(), initial_system.end());

    // Calculate total energy
    double e_kin = utils::getKineticEnergy(system);
    double e_pot = getPotentialEnergy(system);
    const double c_ABS_E_TOT = std::abs(e_kin + e_pot*utils::G);

    // Scale particles to Hénon Units.
    utils::scaleParticles(system, c_ABS_E_TOT);

    // Initialize time step of each particle using Aarseth's initialization formula
    // (MULTIPLE TIME SCALES, 1985)
    for (size_t i = 0; i < system.size(); i++) {
        auto& particle = system.at(i);
        calculateAccJrk(system, particle);

        const double c_ETA = settings.time_step_param;
        particle.dt = c_ETA*particle.acc.norm()/(particle.jrk.norm());
        particle_order[particle.dt] = i; // particle.t = 0.0
    }

    global_time = 0.0;
}

void SM_HITS::evolveSystem()
{
    if (particle_order.empty()) return;

    // Choose first particle in map as min_i(t_i + dt_i)
    const auto c_ORDER_ITERATOR = particle_order.begin();
    global_time = c_ORDER_ITERATOR->first;

    // Retrieve particle index and delete from map
    const double c_NEXT_PARTICLE_INDEX = c_ORDER_ITERATOR->second;
    particle_order.erase(c_ORDER_ITERATOR);

    updateParticle(c_NEXT_PARTICLE_INDEX);

    // Update particle time to global time
    auto& updated_particle = system.at(c_NEXT_PARTICLE_INDEX);
    updated_particle.t = global_time;
    particle_order[updated_particle.t + updated_particle.dt] = c_NEXT_PARTICLE_INDEX;
}

double SM_HITS::getGlobalTime() const { return global_time; }

RenderData SM_HITS::getRenderData() const
{
    return utils::getRenderData(global_time, system);
}

DiagnosticsData SM_HITS::getDiagnosticsData() const
{
    auto synced_system = getPredictedSystem(system, global_time, true);

    const double c_E_POT = getPotentialEnergy(synced_system);

    return utils::getDiagnosticsData(global_time, c_E_POT, synced_system);
}

AnalyticsData SM_HITS::getAnalyticsData() const
{
    auto synced_system = getPredictedSystem(system, global_time, true);

    return utils::getAnalyticsData(global_time, synced_system);
}

//------------------------------------------------------------------------------------------

void SM_HITS::updateParticle(size_t particle_index)
{
    auto& particle_i = system.at(particle_index);

    // Predictor
    auto pred_system = getPredictedSystem(system, global_time);

    // Evaluator
    calculateAccJrk(pred_system, pred_system.at(particle_index));

    // Corrector
    correctParticle(particle_i, pred_system.at(particle_index));

    // Update time step
    updateParticleDt(particle_i);
}

SM_HITS::System SM_HITS::getPredictedSystem( const System& system, double time
                                           , bool sync_mode ) const
{
    System pred_system = system;

    for (auto& pred_particle : pred_system) {
        const double c_DT = time - pred_particle.t;
        const double c_DT2 = c_DT*c_DT;
        const double c_DT3 = c_DT2*c_DT;
        const double c_DT4 = c_DT3*c_DT;
        const double c_DT5 = c_DT4*c_DT;

        pred_particle.pos +=   pred_particle.vel*c_DT
                             + pred_particle.acc*c_DT2/2.0
                             + pred_particle.jrk*c_DT3/6.0;

        pred_particle.vel +=   pred_particle.acc*c_DT
                             + pred_particle.jrk*c_DT2/2.0;

        if (!sync_mode) continue;
        // If the particles need to be synced for synchronisation, we need to use
        // higher order terms for our taylor series

        pred_particle.pos +=   pred_particle.snp*c_DT4/24.0
                             + pred_particle.crk*c_DT5/120.0;

        pred_particle.vel +=   pred_particle.snp*c_DT3/6.0
                             + pred_particle.crk*c_DT4/24.0;
    }

    return pred_system;
}

void SM_HITS::calculateAccJrk(const System& system, Particle& particle_i)
{
    particle_i.acc.fill(0.0);
    particle_i.jrk.fill(0.0);

    for (const auto& particle_j : system) {
        if (&particle_i == &particle_j) continue;

        // Acceleration
        const math::Vector3D c_POS_JI = particle_j.pos - particle_i.pos;
        const double c_DIST_SQR = c_POS_JI.norm2() + c_SOFT_SQR;
        const double c_DIST_CUB = std::sqrt(c_DIST_SQR)*c_DIST_SQR;

        particle_i.acc += c_POS_JI*particle_j.mass/c_DIST_CUB;

        // Jerk
        const math::Vector3D c_VEL_JI = particle_j.vel - particle_i.vel;
        const double c_RV = math::dotProduct(c_POS_JI, c_VEL_JI)/c_DIST_SQR;
        const math::Vector3D c_JRK_TERM = (c_VEL_JI - c_POS_JI*c_RV*3.0);

        particle_i.jrk += c_JRK_TERM*particle_j.mass/c_DIST_CUB;
    }
}

void SM_HITS::correctParticle(Particle& particle, const Particle& pred_particle)
{
    const double c_DT = global_time - particle.t;
    const double c_DT2 = c_DT*c_DT;
    const double c_DT3 = c_DT2*c_DT;

    // Hermite interpolation of snap multiplied by c_DT3 as to avoid higher
    // powers of c_DT
    const math::Vector3D c_SNP_DT3 =   (particle.acc     - pred_particle.acc    )*c_DT*(-6.0)
                                   - (particle.jrk*4.0 + pred_particle.jrk*2.0)*c_DT2;

    // Hermite interpolation of crackle multiplied by c_DT3 as to avoid higher
    // powers of c_DT
    const math::Vector3D c_CRK_DT3 =   (particle.acc - pred_particle.acc)*12.0
                                   + (particle.jrk + pred_particle.jrk)*6.0*c_DT;

    particle.pos = pred_particle.pos + c_SNP_DT3*c_DT/24.0 + c_CRK_DT3*c_DT2/120.0;
    particle.vel = pred_particle.vel + c_SNP_DT3/6.0 + c_CRK_DT3*c_DT/24.0;

//    particle.vel +=   (particle.acc + pred_particle.acc)*c_DT/2
//                    + (particle.jrk - pred_particle.jrk)*c_DT2/12;

//    particle.pos +=   (particle.vel + pred_particle.vel)*c_DT/2
//                    + (particle.acc - pred_particle.acc)*c_DT2/12;

    particle.acc = pred_particle.acc;
    particle.jrk = pred_particle.jrk;
    particle.snp = c_SNP_DT3/c_DT3 + c_CRK_DT3/c_DT2;
    particle.crk = c_CRK_DT3/c_DT3;
}

void SM_HITS::updateParticleDt(Particle& p)
{
    const double c_A = p.acc.norm()*p.snp.norm() + p.jrk.norm2();
    const double c_B = p.jrk.norm()*p.crk.norm() + p.snp.norm2();

    const double c_NEW_DT = std::sqrt(c_A*settings.time_step_param/c_B);

    // Set an upper limit to the relative change of dt
    const double c_MU = 0.3; // allow for 20 % change
    const double c_UPPER_LIMIT = p.dt*(1 + c_MU);
    p.dt = std::min(c_UPPER_LIMIT, c_NEW_DT);

//    // Set an upper and lower limit to the absolute dt
//    const double c_ETA = 20.0;
//    const double c_MIN_DT = settings.time_step_param/c_ETA;
//    const double c_MAX_DT = settings.time_step_param*c_ETA;
//    p.dt = std::max(c_MIN_DT, std::min(c_MAX_DT, p.dt));
}

double SM_HITS::getPotentialEnergy(const System& system) const
{
    double e_pot = 0.0;

    // Sum pair-wise potential energy
    for (size_t i = 0; i < system.size(); i++) {
        if (is_abortion_requested) return e_pot;
        auto& particle_i = system.at(i);

        for (size_t j = i + 1; j < system.size(); j++) {
            auto& particle_j = system.at(j);

            const math::Vector3D c_POS_JI = particle_j.pos - particle_i.pos;
            const double c_DIST_SQR = c_POS_JI.norm2() + c_SOFT_SQR;

            e_pot -= particle_i.mass*particle_j.mass/std::sqrt(c_DIST_SQR);
        }
    }

    return e_pot;
}
