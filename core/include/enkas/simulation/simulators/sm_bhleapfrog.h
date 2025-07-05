#pragma once

#include <vector>
#include <thread>

#include "simulator.h"
#include "utils.h"
#include "render_data.h"
#include "diagnostics_data.h"
#include "analytics_data.h"
#include "bh_tree.h"

class SM_BHLeapfrog : public Simulator
{
public:
    struct Settings
    {
        double time_step;
        double MAC;
        double soft_param;

        bool isValid() const
        {
            return (time_step > 0.0 && MAC >= 0.0 && soft_param > 0.0);
        }
    };

public:
    SM_BHLeapfrog(const Settings& settings);
    ~SM_BHLeapfrog();

    void initializeSystem(const utils::InitialSystem& initial_system) override;
    void evolveSystem() override;

    double getGlobalTime() const override;
    RenderData getRenderData() const override;
    DiagnosticsData getDiagnosticsData() const override;
    AnalyticsData getAnalyticsData() const override;

private:
    using System = std::vector<BHParticle>;

private:
    std::shared_ptr<System> system;

    BHTree bh_tree;

    Settings settings;
    double global_time;      // current time of the system
    const double c_SOFT_SQR; // squared softening parameters
};
