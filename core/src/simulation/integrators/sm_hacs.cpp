#include <algorithm>
#include <unordered_set>

#include "sm_hacs.h"
#include "utils.h"

SM_HACS::SM_HACS(const Settings& settings)
    : settings(settings)
    , global_time(0.0)
    , c_SOFT_SQR(settings.soft_param*settings.soft_param)
{}

//------------------------------------------------------------------------------------------

void SM_HACS::initializeSystem(const utils::InitialSystem& initial_system)
{
    // Scales particles to Hénon Units. Convert BaseParticles to Particles first to
    // calculate potential and kinetic energy
    system.clear();
    for (const auto& particle : initial_system) {
        system.push_back(Particle(particle, settings.max_neighbors));
    }

    // Calculate total energy
    double e_kin = utils::getKineticEnergy(system);
    double e_pot = getPotentialEnergy(system);
    const double c_ABS_E_TOT = std::abs(e_kin + e_pot*utils::G);

    // Scale particles to Hénon units
    utils::scaleParticles(system, c_ABS_E_TOT);

    // Initialize neighbors
    for (size_t i = 0; i < system.size(); i++) {
        updateNeighborList(system, i);
    }

    initializeDerivatives();
    initializeTimeSteps();

    global_time = 0.0;
}

void SM_HACS::evolveSystem()
{
    if (particle_order.empty()) return;

    // Choose first particle in map as min_i(t_i + dt_i)
    const auto c_ORDER_ITERATOR = particle_order.begin();
    global_time = c_ORDER_ITERATOR->first;

    // Retrieve particle index and delete from map
    const double c_MIN_PARTICLE_INDEX = c_ORDER_ITERATOR->second;
    particle_order.erase(c_ORDER_ITERATOR);

    Particle& min_particle = system.at(c_MIN_PARTICLE_INDEX);

    irregularStep(c_MIN_PARTICLE_INDEX);
    min_particle.irr_t = global_time;

    if (min_particle.reg_t + min_particle.reg_dt <= global_time) {
        regularStep(c_MIN_PARTICLE_INDEX);

        min_particle.reg_t = global_time;
        updateRegularTimeStep(min_particle);
    } else {
        const double c_TIME_STEP = global_time - min_particle.reg_t;
        min_particle.acc =   min_particle.irr_acc + min_particle.reg_acc
                           + min_particle.reg_jrk*c_TIME_STEP;
        min_particle.jrk =   min_particle.irr_jrk + min_particle.reg_jrk;
    }

    updateIrregularTimeStep(min_particle);

    // Insert min_particle into map with new time
    particle_order[min_particle.irr_t + min_particle.irr_dt] = c_MIN_PARTICLE_INDEX;
}

double SM_HACS::getGlobalTime() const { return global_time; }

RenderData SM_HACS::getRenderData() const
{
    return utils::getRenderData(global_time, system);
}

DiagnosticsData SM_HACS::getDiagnosticsData() const
{
    auto synced_system = getSyncedSystem();

    const double c_E_POT = getPotentialEnergy(synced_system);

    return utils::getDiagnosticsData(global_time, c_E_POT, synced_system);
}

AnalyticsData SM_HACS::getAnalyticsData() const
{
    auto synced_system = getSyncedSystem();

    return utils::getAnalyticsData(global_time, synced_system);
}


void SM_HACS::initializeDerivatives()
{
    // Initialize irregular forces
    for (auto& particle : system) {
        for (auto& n: particle.neighbors) {
            const auto& neighbor = system.at(n);
            getAccJrk(particle, neighbor, particle.irr_acc, particle.irr_jrk);

            const PairDifferences c_DIFFERENCES =
            {
                {neighbor.pos - particle.pos},
                {neighbor.vel - particle.vel},
                {neighbor.acc - particle.acc},
                {neighbor.jrk - particle.jrk}
            };

            math::Vector3D french; // garbage
            addJrkSnpCrk( particle, neighbor, c_DIFFERENCES
                        , french, particle.irr_snp, particle.irr_crk );
        }
    }

    // Initialize total and regular forces
    for (size_t i = 0; i < system.size(); i++) {
        Particle& particle = system.at(i);

        // Calculate total acc and jrk and use it to get regular acc and jrk
        sumAccJrk(system, i, particle.acc, particle.jrk);
        particle.reg_acc = particle.acc - particle.irr_acc;
        particle.reg_jrk = particle.jrk - particle.irr_jrk;

        // Save total snp and crk in regular variables and subtract irregular
        // part afterwards
        for (const auto& particle_j : system) {
            if (&particle == &particle_j) continue;

            const PairDifferences c_DIFFERENCES =
            {
                {particle_j.pos - particle.pos},
                {particle_j.vel - particle.vel},
                {particle_j.acc - particle.acc},
                {particle_j.jrk - particle.jrk}
            };

            math::Vector3D cross_product; // fuck em, we wedging
            addJrkSnpCrk( particle, particle_j, c_DIFFERENCES
                        , cross_product, particle.reg_snp, particle.reg_crk );
        }

        particle.reg_snp -= particle.irr_snp;
        particle.reg_crk -= particle.irr_crk;
    }
}

void SM_HACS::initializeTimeSteps()
{
    for (size_t i = 0; i < system.size(); i++) {
        auto& particle = system.at(i);

        updateIrregularTimeStep(particle);
        updateRegularTimeStep(particle);

        // Check for alternative regular time_step according to Aarseth's formula
        // (Publications of the Astronomical Society of Japan, v.44, p.141-151, 1992)
        const double c_ETA = particle.vel.norm2();
        if (c_ETA > 0.0) {
            double alternative_reg_dt = 0.1*std::sqrt(particle.neighbor_radius2/c_ETA);
            particle.reg_dt = std::min(particle.reg_dt, alternative_reg_dt);
        }

        // Initialize map
        particle_order[particle.irr_dt] = i; // particle.t = 0.0
    }
}

void SM_HACS::updateIrregularTimeStep(Particle& p)
{
    const double c_A = (    p.acc.norm()*p.irr_snp.norm() + p.irr_jrk.norm2());
    const double c_B = (p.irr_jrk.norm()*p.irr_crk.norm() + p.irr_snp.norm2());

    p.irr_dt = std::sqrt(settings.irr_step_param*c_A/c_B);
}

void SM_HACS::updateRegularTimeStep(Particle& p)
{
    const double c_A = (p.reg_acc.norm()*p.reg_snp.norm() + p.reg_jrk.norm2());
    const double c_B = (p.reg_jrk.norm()*p.reg_crk.norm() + p.reg_snp.norm2());

    p.reg_dt = std::sqrt(settings.reg_step_param*c_A/c_B);
}

void SM_HACS::irregularStep(size_t particle_index)
{
    Particle& particle = system.at(particle_index);


    // Predictor -------------------------------------------------------------
    // Create container of neighboring particles
    System pred_neighbors;
    pred_neighbors.reserve(particle.neighbors.size() + 1);
    for (auto neighbor_index: particle.neighbors) {
        pred_neighbors.push_back(system.at(neighbor_index));
    }

    // Add particle_i to the end of neighbor list
    pred_neighbors.push_back(particle);

    // Predict neighbors: pos and vel
    predictParticles(pred_neighbors, global_time);


    // Evaluator: Evaluate neighbor forces of pred_particle ------------------
    // Index of pred_particle in pred_neighbors
    const size_t c_PPI_IDX = pred_neighbors.size() - 1;
    Particle pred_particle = pred_neighbors.at(c_PPI_IDX);

    sumAccJrk(pred_neighbors, c_PPI_IDX, pred_particle.irr_acc, pred_particle.irr_jrk);


    // Corrector: update pos, vel and neighbor derivatives --------------------
    irregularCorrection(particle, pred_particle);
}

void SM_HACS::irregularCorrection(Particle& particle, const Particle& pred_particle)
{
    const double c_DT = particle.irr_dt;
    const double c_DT2 = c_DT*c_DT;
    const double c_DT3 = c_DT2*c_DT;

    // Hermite interpolation of irregular snap multiplied by (c_DT3 as to
    // avoid higher powers of c_DT)
    const math::Vector3D c_IRR_SNP_DT3 =
          (particle.irr_acc   - pred_particle.irr_acc  )*c_DT*(-6)
        - (particle.irr_jrk*4 + pred_particle.irr_jrk*2)*c_DT2;

    // Hermite interpolation of irregular crackle (multiplied by c_DT3 as to
    // avoid higher powers of c_DT)
    const math::Vector3D c_IRR_CRK_DT3 =
          (particle.irr_acc - pred_particle.irr_acc)*12
        + (particle.irr_jrk + pred_particle.irr_jrk)*6*c_DT;

    // Correct the predicted position and velocity of particle_i
    particle.pos =   pred_particle.pos
                   + c_IRR_SNP_DT3*c_DT/24
                   + c_IRR_CRK_DT3*c_DT2/120;

    particle.vel =   pred_particle.vel
                   + c_IRR_SNP_DT3/6
                   + c_IRR_CRK_DT3*c_DT/24;

    // Update irregular derivatives
    particle.irr_acc = pred_particle.irr_acc;
    particle.irr_jrk = pred_particle.irr_jrk;
    particle.irr_snp = c_IRR_SNP_DT3/c_DT3 + c_IRR_CRK_DT3/c_DT2;
    particle.irr_crk = c_IRR_CRK_DT3/c_DT3;
}

void SM_HACS::regularStep(size_t particle_index)
{
    Particle& particle = system.at(particle_index);


    // Predictor ------------------------------------------------------------------------

    // Predict all particles at global_time
    System pred_system = system;
    predictParticles(pred_system, global_time);
    Particle pred_particle = pred_system.at(particle_index);

    // Update list of neighbors
    updateNeighborList(pred_system, particle_index);

    // Index of all particles which are a neighbor according to the new list
    std::unordered_set<size_t> new_neighbors( pred_particle.neighbors.begin()
                                            , pred_particle.neighbors.end() );

    // Index of all particles which are a neighbor according to the old list
    std::unordered_set<size_t> old_neighbors( particle.neighbors.begin()
                                            , particle.neighbors.end() );

    // Stores indices of neighbors which entered or left the neighbor sphere inside
    // neighbor_flux. c_FIRST_NEW_IDX determines the index of the first new neighbor
    std::vector<size_t> neighbor_flux;

    // Add all neighbor indices to neighbor_flux which have left the neighbor list
    std::set_difference(old_neighbors.begin(), old_neighbors.end(),
                        new_neighbors.begin(), new_neighbors.end(),
                        std::back_inserter(neighbor_flux));

    // Add all neighbor indices to neighbor_flux which have left the neighbor list
    const size_t c_FIRST_NEW_IDX = neighbor_flux.size();

    // Add all neighbor indices to neighbor_flux which have entered the neighbor list
    std::set_difference(new_neighbors.begin(), new_neighbors.end(),
                        old_neighbors.begin(), old_neighbors.end(),
                        std::back_inserter(neighbor_flux));

    // Update the neighbor list of particle
    particle.neighbors = pred_particle.neighbors;


    // Evaluator ------------------------------------------------------------------------

    // Store total acc and jerk at time t1 in regular variables of pred_particle
    // We'll subtract the irregular part later
    sumAccJrk( pred_system, particle_index
             , pred_particle.reg_acc, pred_particle.reg_jrk );

    // Create container of neighbors with corrected pos and vel with old list of indices
    std::vector<Particle> pred_old_neighbors;
    pred_old_neighbors.reserve(particle.neighbors.size() + 1);
    for (auto neighbor_index: particle.neighbors) {
        pred_old_neighbors.push_back(pred_system.at(neighbor_index));
    }

    // Calculate new neighbor acc and jrk at time t1
    pred_old_neighbors.push_back(pred_particle);
    sumAccJrk( pred_old_neighbors, pred_old_neighbors.size() - 1
             , pred_particle.irr_acc, pred_particle.irr_jrk );

    // Subtract the irregular part of the total acc and jrk we stored in the regular
    // variables of pred_particle two steps ago
    pred_particle.reg_acc -= pred_particle.irr_acc;
    pred_particle.reg_jrk -= pred_particle.irr_jrk;


    // Corrector 1: update pos, vel and neighbor derivatives ----------------------------
    regularCorrection(particle, pred_particle);


    // Corrector 2: correct derivatives based on change of neighbors --------------------
    neighborCorrection( particle, pred_particle, pred_system
                      , neighbor_flux, c_FIRST_NEW_IDX );
}

void SM_HACS::regularCorrection(Particle& particle, const Particle& pred_particle)
{
    const double c_DT = global_time - particle.reg_t;
    const double c_DT2 = c_DT;
    const double c_DT3 = c_DT2*c_DT;

    const math::Vector3D c_ACC_DIFF =   (particle.reg_acc - pred_particle.reg_acc)
                                    - (pred_particle.irr_acc - particle.irr_acc);

    const math::Vector3D c_OLD_JRK = particle.reg_jrk + pred_particle.reg_jrk;
    const math::Vector3D c_NEW_JRK = pred_particle.irr_jrk - particle.irr_jrk;

    // Hermite interpolation of irregular snap multiplied by c_DT3 as to
    // avoid higher powers of c_DT
    const math::Vector3D c_REG_SNP_DT3 =   c_ACC_DIFF*c_DT*(-6)
                                       - (c_OLD_JRK*4 + c_NEW_JRK*2)*c_DT2;

    // Hermite interpolation of irregular crackle multiplied by c_DT3 as to
    // avoid higher powers of c_DT
    const math::Vector3D c_REG_CRK_DT3 = c_ACC_DIFF*12 + (c_OLD_JRK + c_NEW_JRK)*6*c_DT;

    particle.pos += c_REG_SNP_DT3*c_DT/24 + c_REG_CRK_DT3*c_DT2/120;
    particle.vel += c_REG_SNP_DT3/6 + c_REG_CRK_DT3*c_DT/24;

    particle.irr_acc = pred_particle.irr_acc;
    particle.irr_jrk = pred_particle.irr_jrk;

    particle.reg_acc = pred_particle.reg_acc;
    particle.reg_jrk = pred_particle.reg_jrk;
    particle.reg_snp = c_REG_SNP_DT3/c_DT3 + c_REG_CRK_DT3/c_DT2;
    particle.reg_crk = c_REG_CRK_DT3/c_DT3;

    particle.acc = pred_particle.irr_acc + pred_particle.reg_acc;
    particle.jrk = pred_particle.irr_jrk + pred_particle.reg_jrk;
}

void SM_HACS::neighborCorrection( Particle& particle, const Particle& pred_particle
                                , const std::vector<Particle> pred_system
                                , const std::vector<size_t> neighbor_flux
                                , size_t c_FIRST_NEW_IDX )
{
    if (neighbor_flux.size() == 0) return;

    for (size_t k = 0; k < neighbor_flux.size(); k++) {
        const auto& neighbor = pred_system.at(neighbor_flux.at(k));
        const auto& c_DT = global_time - neighbor.irr_t;
        const auto& c_DT2 = c_DT*c_DT;

        const PairDifferences c_DIFFERENCES =
        {
            {neighbor.pos - pred_particle.pos},
            {neighbor.vel - pred_particle.vel + neighbor.acc*c_DT + neighbor.jrk*c_DT2/2},
            {neighbor.acc - pred_particle.acc + neighbor.jrk*c_DT},
            {neighbor.jrk - pred_particle.jrk}
        };

        math::Vector3D corr_jrk_neighbour;
        math::Vector3D corr_snp_neighbour;
        math::Vector3D corr_crk_neighbour;

        addJrkSnpCrk( pred_particle, neighbor, c_DIFFERENCES
                    , corr_jrk_neighbour, corr_snp_neighbour, corr_crk_neighbour );

        int sign = k < c_FIRST_NEW_IDX ? -1 : 1;

        const math::Vector3D c_JRK_CORRECTION = corr_jrk_neighbour*sign;
        const math::Vector3D c_SNP_CORRECTION = corr_snp_neighbour*sign;
        const math::Vector3D c_CRK_CORRECTION = corr_crk_neighbour*sign;

        particle.irr_jrk += c_JRK_CORRECTION;
        particle.irr_snp += c_SNP_CORRECTION;
        particle.irr_crk += c_CRK_CORRECTION;

        particle.reg_jrk -= c_JRK_CORRECTION;
        particle.reg_snp -= c_SNP_CORRECTION;
        particle.reg_crk -= c_CRK_CORRECTION;
    }
}

void SM_HACS::updateNeighborList(System& system, size_t i)
{
    Particle& particle_i = system.at(i);

    double search_radius2 = particle_i.neighbor_radius2;

    bool stop_search = false;
    while (!stop_search) {
        particle_i.neighbors.clear();

        for (size_t j = 0; j < system.size(); j++) {
            if (j == i) continue;

            if ((system.at(j).pos - particle_i.pos).norm2() <= search_radius2)
                particle_i.neighbors.push_back(j);
        }

        const int c_NUM_NEIGHBORS = particle_i.neighbors.size();

        // Increase search radius if no neighbors were found
        if (c_NUM_NEIGHBORS == 0) search_radius2 *= 1.59;

        // Ensure there aren't more neighbors inside the neighbor list
        // than expected by ac_max_num by reducing the search radius
        if (c_NUM_NEIGHBORS > settings.max_neighbors) {
            double shrink_factor = 0.75*settings.max_neighbors/c_NUM_NEIGHBORS;
            if (std::abs(shrink_factor - 0.5) < 0.05 ) shrink_factor *= 1.2;
            search_radius2 *= shrink_factor*0.67;
        } else {
            stop_search = true;
        }
    }

    particle_i.neighbor_radius2 = search_radius2;
}

void SM_HACS::predictParticles(System& system, double time)
{
    for (auto& pred_particle: system) {
        const double c_DT = time - pred_particle.irr_t;
        const double c_DT2 = c_DT*c_DT;
        const double c_DT3 = c_DT2*c_DT;

        const math::Vector3D pred_particle_acc = pred_particle.irr_acc + pred_particle.reg_acc;
        const math::Vector3D pred_particle_jrk = pred_particle.irr_jrk + pred_particle.reg_jrk;

        pred_particle.pos +=   pred_particle.vel*c_DT
                             + pred_particle_acc*c_DT2/2
                             + pred_particle_jrk*c_DT3/6;

        pred_particle.vel +=   pred_particle_acc*c_DT
                             + pred_particle_jrk*c_DT2/2;
    }
}

void SM_HACS::sumAccJrk( const System& system, const size_t i
                       , math::Vector3D& acc, math::Vector3D& jrk)
{
    acc.fill(0.0);
    jrk.fill(0.0);

    for (const auto& particle_j : system) {
        getAccJrk(system.at(i), particle_j, acc, jrk);
    }
}

void SM_HACS::getAccJrk( const Particle& particle_i, const Particle& particle_j
                       , math::Vector3D& acc, math::Vector3D& jrk )
{
    const math::Vector3D c_POS_JI = particle_j.pos - particle_i.pos;
    const math::Vector3D c_VEL_JI = particle_j.vel - particle_i.vel;

    const double c_DIST_SQR = c_POS_JI.norm2() + c_SOFT_SQR;
    const double c_DIST_CUB = std::sqrt(c_DIST_SQR)*c_DIST_SQR;
    const double c_RV = c_POS_JI.norm()*c_VEL_JI.norm()/c_DIST_SQR;

    acc += c_POS_JI*particle_j.mass/c_DIST_CUB;
    jrk += (c_VEL_JI - c_POS_JI*c_RV*3.0)*particle_j.mass/c_DIST_CUB;
}

void SM_HACS::addJrkSnpCrk( const Particle& particle_i, const Particle& particle_j
                          , const PairDifferences& d
                          , math::Vector3D& jrk, math::Vector3D& snp, math::Vector3D& crk )
{
    // I'm not proud of this function

    const double c_DIST_SQR = d.pos_ji.norm2() + c_SOFT_SQR;
    const double c_MASS_FACTOR = particle_j.mass/std::sqrt(c_DIST_SQR)/c_DIST_SQR;
    const double c_RV = d.pos_ji.norm()*d.vel_ji.norm()/c_DIST_SQR;
    const double c_RV2 = c_RV*c_RV;
    const double c_RV3 = c_RV2*c_RV;

    const math::Vector3D c_A = (d.vel_ji - d.pos_ji*c_RV*3.0);
    const double c_B = (d.vel_ji.norm2() + d.pos_ji.norm()*d.acc_ji.norm())/c_DIST_SQR + c_RV2;
    const double c_E = d.pos_ji.norm()*d.jrk_ji.norm()*3.0;
    const double c_C = d.vel_ji.norm()*d.acc_ji.norm()*9.0 + c_E;
    const double c_D = c_C/c_DIST_SQR + c_B*c_RV*3.0 - c_RV3*12.0;

    jrk += c_A*c_MASS_FACTOR;
    snp += (d.acc_ji - c_A*c_RV*6.0 - d.pos_ji*c_B*3.0)*c_MASS_FACTOR;
    crk += (d.jrk_ji - c_A*c_B*9.0 - d.pos_ji*c_D)*c_MASS_FACTOR - snp*c_RV*9.0;
}

std::vector<SM_HACS::Particle> SM_HACS::getSyncedSystem() const
{
    System synced_system = system;

    for (auto& synced_particle : synced_system) {
        const double c_DTIR  = synced_particle.irr_t - synced_particle.reg_t;
        const double c_DTIR2 = c_DTIR*c_DTIR;
        const double c_DTIR3 = c_DTIR2*c_DTIR;

        const math::Vector3D c_ACC =   synced_particle.irr_acc + synced_particle.reg_acc
                                   + synced_particle.reg_jrk*c_DTIR
                                   + synced_particle.reg_snp*c_DTIR2/2
                                   + synced_particle.reg_crk*c_DTIR3/6;

        const math::Vector3D c_JRK =   synced_particle.irr_jrk + synced_particle.reg_jrk
                                   + synced_particle.reg_snp*c_DTIR
                                   + synced_particle.reg_crk*c_DTIR2/2;

        const math::Vector3D c_SNP =   synced_particle.irr_snp + synced_particle.reg_snp
                                   + synced_particle.reg_crk*c_DTIR;

        const math::Vector3D c_CRK = synced_particle.irr_crk + synced_particle.reg_crk;

        const double c_DT  = global_time - synced_particle.irr_t;
        const double c_DT2 = c_DT*c_DT;
        const double c_DT3 = c_DT2*c_DT;
        const double c_DT4 = c_DT3*c_DT;
        const double c_DT5 = c_DT4*c_DT;

        synced_particle.pos +=   synced_particle.vel*c_DT
                               + c_ACC*c_DT2/2
                               + c_JRK*c_DT3/6
                               + c_SNP*c_DT4/24
                               + c_CRK*c_DT5/120;

        synced_particle.vel +=   c_ACC*c_DT
                               + c_JRK*c_DT2/2
                               + c_SNP*c_DT3/6
                               + c_CRK*c_DT4/24;
    }

    return synced_system;
}

double SM_HACS::getPotentialEnergy(const System& system) const
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
