#pragma once

#include <QString>

#include "views/new_simulation_tab/settings_widgets/setting_descriptor.h"
#include "views/new_simulation_tab/settings_widgets/settings_schema.h"

class UniformCubeSchema : public SettingsSchema {
public:
    QString name() const override { return "Uniform Cube"; }
    QVector<SettingDescriptor> settingsSchema() const override {
        return {{SettingKey::UniformCubeSeed,
                 "Seed",
                 SettingDescriptor::RandomInt,
                 {},
                 limits::random_min,
                 limits::random_max},
                {SettingKey::UniformCubeParticleCount,
                 "Particle Count",
                 SettingDescriptor::Int,
                 1000,
                 limits::particle_count_min,
                 limits::particle_count_max},
                {SettingKey::UniformCubeSideLength,
                 "Side Length",
                 SettingDescriptor::Double,
                 1.0,
                 limits::smallest_greater_than_zero,
                 limits::double_max},
                {SettingKey::UniformCubeInitialVelocity,
                 "Initial Velocity",
                 SettingDescriptor::Double,
                 0.0,
                 limits::zero,
                 limits::double_max},
                {SettingKey::UniformCubeTotalMass,
                 "Total Mass",
                 SettingDescriptor::Double,
                 1000.0,
                 limits::smallest_greater_than_zero,
                 limits::double_max}};
    }
};
