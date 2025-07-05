#pragma once

#include <vector>
#include <map>

#include "utils.h"
#include "simulator.h"
#include "render_data.h"
#include "diagnostics_data.h"
#include "analytics_data.h"


class SM_HACS : public Simulator
{
public:
    struct Settings
    {
        double irr_step_param;
        double reg_step_param;
        int max_neighbors;
        double soft_param;

        bool isValid() const
        {
            return ( irr_step_param > 0.0 && reg_step_param > 0.0
                    && soft_param > 0.0 && max_neighbors >= 0.0 );
        }
    };

public:
    SM_HACS(const Settings& settings);

    void initializeSystem(const utils::InitialSystem& initial_system) override;
    void evolveSystem() override;

    double getGlobalTime() const override;
    RenderData getRenderData() const override;
    DiagnosticsData getDiagnosticsData() const override;
    AnalyticsData getAnalyticsData() const override;

private:
    struct Particle : public utils::BaseParticle
    {
        math::Vector3D acc; // acceleration
        math::Vector3D jrk; // jerk

        math::Vector3D irr_acc; // acceleration of the irregular step
        math::Vector3D irr_jrk; // jerk of the irregular step
        math::Vector3D irr_snp; // snap of the irregular step
        math::Vector3D irr_crk; // crackle of the irregular step

        double irr_t = 0.0; // irregular particle time
        double irr_dt = 0.0; // irregular particle time step

        math::Vector3D reg_acc; // acceleration of the regular step
        math::Vector3D reg_jrk; // jerk of the regular step
        math::Vector3D reg_snp; // snap of the regular step
        math::Vector3D reg_crk; // crackle of the regular step

        double reg_t = 0.0; // regular particle time
        double reg_dt = 0.0; // regular particle time step

        double neighbor_radius2 = 1.0; // neighbour radius squared
        std::vector<size_t> neighbors; // indices of neighbours

        // max size corresponds to ac_max_num in SM_HACS::Settings
        Particle(int max_size)
        { neighbors.reserve(max_size); }
        Particle(const BaseParticle& b, int max_size) : BaseParticle(b)
        { neighbors.reserve(max_size); }
    };

    using System = std::vector<Particle>;

    struct PairDifferences {
        math::Vector3D pos_ji;
        math::Vector3D vel_ji;
        math::Vector3D acc_ji;
        math::Vector3D jrk_ji;
    };

private:
    void initializeDerivatives();
    void initializeTimeSteps();

    /**
     * @brief Updates a particle's irregular timestep
     *
     * @param particle Particle to be updated
     *
     * @see Aarseth's timestep formula (Publications of the Astronomical Society
     *      of Japan, v.44, p.141-151, 1992)
     */
    void updateIrregularTimeStep(Particle& particle);

    /**
     * @brief Updates a particle's regular timestep
     *
     * @param particle Particle to be updated
     *
     * @see Aarseth's timestep formula (Publications of the Astronomical Society
     *      of Japan, v.44, p.141-151, 1992)
     */
    void updateRegularTimeStep(Particle& particle);

    void irregularStep(size_t particle_index);
    void irregularCorrection(Particle& particle, const Particle& pred_particle);
    void regularStep(size_t particle_index);
    void regularCorrection(Particle& particle, const Particle& pred_particle);
    void neighborCorrection( Particle& particle, const Particle& pred_particle
                           , const System pred_system
                           , const std::vector<size_t> neighbor_flux
                           , size_t c_FIRST_NEW_IDX );

    void updateNeighborList(System& particles, size_t i);
    void predictParticles(System& particles, double time);
    void sumAccJrk( const System& particles, size_t i
                  , math::Vector3D& acc, math::Vector3D& jrk );
    void getAccJrk( const Particle& particle_i, const Particle& particle_j
                  , math::Vector3D& acc, math::Vector3D& jrk );
    void addJrkSnpCrk( const Particle& particle_i, const Particle& particle_j
                     , const PairDifferences& differences
                     , math::Vector3D& jrk, math::Vector3D& snp, math::Vector3D& crk );

    System getSyncedSystem() const;
    double getPotentialEnergy(const System& system) const;

private:

    System system;
    std::map<double, size_t> particle_order;

    Settings settings;

    double global_time;      // current time of the system
    const double c_SOFT_SQR; // squared softening parameters
};
