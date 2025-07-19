#pragma once

#include <QString>

#include "views/new_simulation_tab/settings_widgets/setting_descriptor.h"
#include "views/new_simulation_tab/settings_widgets/settings_schema.h"

class HermiteSchema : public SettingsSchema {
public:
    QString name() const override { return "Hermite"; }
    QVector<SettingDescriptor> settingsSchema() const override {
        return {{SettingKey::HermiteTimeStep,
                 "Time step",
                 SettingDescriptor::Double,
                 0.01,
                 limits::smallest_greater_than_zero,
                 limits::double_max},
                {SettingKey::HermiteSoftening,
                 "Softening",
                 SettingDescriptor::Double,
                 0.001,
                 limits::smallest_greater_than_zero,
                 limits::double_max}};
    }
};
