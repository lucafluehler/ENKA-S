#pragma once

#include <QString>

#include "views/new_simulation_tab/settings_widgets/setting_descriptor.h"
#include "views/new_simulation_tab/settings_widgets/settings_schema.h"

class HitsSchema : public SettingsSchema {
public:
    QString name() const override { return "Hermite Individual Time Step"; }
    QVector<SettingDescriptor> settingsSchema() const override {
        return {{SettingKey::HitsTimeStepParam,
                 "Time step parameter",
                 SettingDescriptor::Double,
                 0.0001,
                 limits::smallest_greater_than_zero,
                 limits::double_max},
                {SettingKey::HitsSoftening,
                 "Softening",
                 SettingDescriptor::Double,
                 0.001,
                 limits::smallest_greater_than_zero,
                 limits::double_max}};
    }
};
