#ifndef SM_HITS_H_
#define SM_HITS_H_

#include <vector>
#include <map>

#include "utils.h"
#include "simulator.h"
#include "render_data.h"
#include "diagnostics_data.h"
#include "analytics_data.h"

class SM_HITS : public Simulator
{
public:
    struct Settings
    {
        double time_step_param = 0.0;
        double soft_param = 0.0; // softening parameter

        bool isValid() const
        {
            return (time_step_param != 0.0 && soft_param != 0.0);
        }
    };

public:
    SM_HITS(const Settings& settings);

    void initializeSystem(const utils::InitialSystem& initial_system) override;
    void evolveSystem() override;

    double getGlobalTime() const override;
    RenderData getRenderData() const override;
    DiagnosticsData getDiagnosticsData() const override;
    AnalyticsData getAnalyticsData() const override;

private:
    struct Particle : public utils::BaseParticle
    {
        ga::Vector3D acc; // acceleration
        ga::Vector3D jrk; // jerk
        ga::Vector3D snp; // snap; necessary for global synchronization
        ga::Vector3D crk; // crackle; necessary for global synchronization

        double t = 0.0; // particle time
        double dt = 0.0; // particle time step
    };

    using System = std::vector<Particle>;

private:
    void updateParticle(size_t particle_index);
    System getPredictedSystem( const System& system, double time
                             , bool sync_mode = false ) const;
    void calculateAccJrk(const System& system, Particle& particle_i);
    void correctParticle(Particle& particle, const Particle& pred_particle);
    void updateParticleDt(Particle& particle);

    double getPotentialEnergy(const System& system) const;

private:
    System system;
    std::map<double, size_t> particle_order;

    Settings settings;

    double global_time;      // current time of the system
    const double c_SOFT_SQR; // squared softening parameters
};

#endif // SM_HITS_H_
