#pragma once

#include <QString>

#include "views/new_simulation_tab/settings_widgets/setting_descriptor.h"
#include "views/new_simulation_tab/settings_widgets/settings_schema.h"

class EulerSchema : public SettingsSchema {
public:
    QString name() const override { return "Euler"; }
    QVector<SettingDescriptor> settingsSchema() const override {
        return {
            {SettingKey::EulerTimeStep, "Time step", SettingDescriptor::Double, 0.01, 1e-15, 1e10},
            {SettingKey::EulerSoftening,
             "Softening",
             SettingDescriptor::Double,
             0.001,
             1e-15,
             1e10}};
    }
};
