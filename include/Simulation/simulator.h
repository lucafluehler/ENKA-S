#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include "utils.h"

#include "render_data.h"
#include "diagnostics_data.h"
#include "analytics_data.h"

class Simulator
{
public:
    virtual void initializeSystem(const utils::InitialSystem& initial_system) = 0;
    virtual void evolveSystem() = 0;

    virtual void requestAbortion() { is_abortion_requested = true; }

    virtual double getGlobalTime() const = 0;
    virtual RenderData getRenderData() const = 0;
    virtual DiagnosticsData getDiagnosticsData() const = 0;
    virtual AnalyticsData getAnalyticsData() const = 0;

protected:
    bool is_abortion_requested = false;
};

#endif // SIMULATOR_H_
