#pragma once

#include <QString>

#include "views/new_simulation_tab/settings_widgets/setting_descriptor.h"
#include "views/new_simulation_tab/settings_widgets/settings_schema.h"

class BarnesHutLeapfrogSchema : public SettingsSchema {
public:
    QString name() const override { return "Barnes-Hut (Leapfrog)"; }
    QVector<SettingDescriptor> settingsSchema() const override {
        return {{SettingKey::BarnesHutLeapfrogTimeStep,
                 "Time step",
                 SettingDescriptor::Double,
                 0.01,
                 limits::smallest_greater_than_zero,
                 limits::double_max},
                {SettingKey::BarnesHutLeapfrogThetaMac,
                 "MAC",
                 SettingDescriptor::Double,
                 0.01,
                 limits::smallest_greater_than_zero,
                 limits::double_max},
                {SettingKey::BarnesHutLeapfrogSoftening,
                 "Softening",
                 SettingDescriptor::Double,
                 0.001,
                 limits::smallest_greater_than_zero,
                 limits::double_max}};
    }
};
