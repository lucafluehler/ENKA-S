#pragma once

#include <QString>

#include "views/new_simulation_tab/settings_widgets/setting_descriptor.h"
#include "views/new_simulation_tab/settings_widgets/settings_schema.h"

class PlummerSphereSchema : public SettingsSchema {
public:
    QString name() const override { return "Plummer Sphere"; }
    QVector<SettingDescriptor> settingsSchema() const override {
        return {{SettingKey::PlummerSphereSeed,
                 "Seed",
                 SettingDescriptor::RandomInt,
                 {},
                 limits::random_min,
                 limits::random_max},
                {SettingKey::PlummerSphereParticleCount,
                 "Particle Count",
                 SettingDescriptor::Int,
                 1000,
                 limits::particle_count_min,
                 limits::particle_count_max},
                {SettingKey::PlummerSphereRadius,
                 "Radius",
                 SettingDescriptor::Double,
                 1.0,
                 limits::smallest_greater_than_zero,
                 limits::double_max},
                {SettingKey::PlummerSphereInitialVelocity,
                 "Initial Velocity",
                 SettingDescriptor::Double,
                 0.0,
                 limits::zero,
                 limits::double_max},
                {SettingKey::PlummerSphereTotalMass,
                 "Total Mass",
                 SettingDescriptor::Double,
                 1000.0,
                 limits::smallest_greater_than_zero,
                 limits::double_max}};
    }
};
