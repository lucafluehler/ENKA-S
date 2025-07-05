#ifndef DATA_PTR_H_
#define DATA_PTR_H_

#include <memory>

#include "settings.h"
#include "utils.h"
#include "render_data.h"
#include "diagnostics_data.h"
#include "analytics_data.h"

struct DataPtr
{
    std::shared_ptr<Settings> settings = nullptr;
    std::shared_ptr<utils::InitialSystem> initial_system = nullptr;

    std::shared_ptr<RenderData> render_data = nullptr;
    std::shared_ptr<DiagnosticsData> diagnostics_data = nullptr;
    std::shared_ptr<AnalyticsData> analytics_data = nullptr;
};

#endif // DATA_PTR_H_
