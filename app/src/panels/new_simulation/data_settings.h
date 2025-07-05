#pragma once

struct DataSettings
{
    double render_step;
    double diagnostics_step;
    double analytics_step;

    bool save_render_data;
    bool save_diagnostics_data;
    bool save_analytics_data;
    bool save_folder;
};
