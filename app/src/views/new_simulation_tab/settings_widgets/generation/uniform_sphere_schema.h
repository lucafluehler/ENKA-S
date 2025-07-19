#pragma once

#include <QString>

#include "views/new_simulation_tab/settings_widgets/setting_descriptor.h"
#include "views/new_simulation_tab/settings_widgets/settings_schema.h"

class UniformSphereSchema : public SettingsSchema {
public:
    QString name() const override { return "Uniform Sphere"; }
    QVector<SettingDescriptor> settingsSchema() const override {
        return {{SettingKey::UniformSphereSeed,
                 "Seed",
                 SettingDescriptor::RandomInt,
                 {},
                 limits::random_min,
                 limits::random_max},
                {SettingKey::UniformSphereParticleCount,
                 "Particle Count",
                 SettingDescriptor::Int,
                 1000,
                 limits::particle_count_min,
                 limits::particle_count_max},
                {SettingKey::UniformSphereRadius,
                 "Radius",
                 SettingDescriptor::Double,
                 1.0,
                 limits::smallest_greater_than_zero,
                 limits::double_max},
                {SettingKey::UniformSphereInitialVelocity,
                 "Initial Velocity",
                 SettingDescriptor::Double,
                 0.0,
                 limits::zero,
                 limits::double_max},
                {SettingKey::UniformSphereTotalMass,
                 "Total Mass",
                 SettingDescriptor::Double,
                 1000.0,
                 limits::smallest_greater_than_zero,
                 limits::double_max}};
    }
};
