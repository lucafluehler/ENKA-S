#pragma once

#include <QString>

#include "views/new_simulation_tab/settings_widgets/setting_descriptor.h"
#include "views/new_simulation_tab/settings_widgets/settings_schema.h"

class NormalSphereSchema : public SettingsSchema {
public:
    QString name() const override { return "Normal Sphere"; }
    QVector<SettingDescriptor> settingsSchema() const override {
        return {{SettingKey::NormalSphereSeed, "Seed", SettingDescriptor::RandomInt, {}, 0.0, 1e10},
                {SettingKey::NormalSphereParticleCount,
                 "Particle Count",
                 SettingDescriptor::Int,
                 1000,
                 1,
                 1e6},
                {SettingKey::NormalSpherePositionStdDev,
                 "Position Std Dev",
                 SettingDescriptor::Double,
                 0.1,
                 0.0,
                 1e10},
                {SettingKey::NormalSphereVelocityStdDev,
                 "Velocity Std Dev",
                 SettingDescriptor::Double,
                 0.1,
                 0.0,
                 1e10},
                {SettingKey::NormalSphereMassMean,
                 "Mass Mean",
                 SettingDescriptor::Double,
                 1.0,
                 0.0,
                 1e10},
                {SettingKey::NormalSphereMassStdDev,
                 "Mass Std Dev",
                 SettingDescriptor::Double,
                 0.1,
                 0.0,
                 1e10}};
    }
};
