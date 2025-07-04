#ifndef SM_HERMITE_H_
#define SM_HERMITE_H_

#include <vector>
#include <map>

#include "utils.h"
#include "simulator.h"
#include "render_data.h"
#include "diagnostics_data.h"
#include "analytics_data.h"

class SM_Hermite : public Simulator
{
public:
    struct Settings
    {
        double time_step = 0.0; // constant global time step
        double soft_param = 0.0; // softening parameter

        bool isValid() const
        {
            return (time_step != 0.0 && soft_param != 0.0);
        }
    };

public:
    SM_Hermite(const Settings& settings);

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
    };

    using System = std::vector<Particle>;

private:
    void calculateAccJrkEpot();

private:
    System system;

    Settings settings;

    double global_time;      // current time of the system
    double e_pot;            // total potential energy of the system
    const double c_SOFT_SQR; // squared softening parameters
};

#endif // SM_HERMITE_H_
