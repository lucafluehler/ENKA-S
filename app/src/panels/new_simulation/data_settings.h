#ifndef DATA_SETTINGS_H_
#define DATA_SETTINGS_H_

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

#endif // DATA_SETTINGS_H_
