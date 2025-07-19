#pragma once

#include <QString>

#include "views/new_simulation_tab/settings_widgets/setting_descriptor.h"
#include "views/new_simulation_tab/settings_widgets/settings_schema.h"

class CollisionModelSchema : public SettingsSchema {
public:
    QString name() const override { return "Collision Model"; }
    QVector<SettingDescriptor> settingsSchema() const override {
        return {{SettingKey::CollisionModelSeed,
                 "Seed",
                 SettingDescriptor::RandomInt,
                 {},
                 limits::random_min,
                 limits::random_max},
                {SettingKey::CollisionModelImpactParameter,
                 "Impact Parameter",
                 SettingDescriptor::Double,
                 0.5,
                 limits::zero,
                 limits::double_max},
                {SettingKey::CollisionModelRelativeVelocity,
                 "Relative Velocity",
                 SettingDescriptor::Double,
                 0.5,
                 limits::zero,
                 limits::double_max},
                {SettingKey::CollisionModelParticleCount1,
                 "Particle Count 1",
                 SettingDescriptor::Int,
                 1000,
                 limits::particle_count_min,
                 limits::particle_count_max},
                {SettingKey::CollisionModelSphereRadius1,
                 "Sphere Radius 1",
                 SettingDescriptor::Double,
                 1.0,
                 limits::smallest_greater_than_zero,
                 limits::double_max},
                {SettingKey::CollisionModelTotalMass1,
                 "Total Mass 1",
                 SettingDescriptor::Double,
                 1000.0,
                 limits::smallest_greater_than_zero,
                 limits::double_max},
                {SettingKey::CollisionModelParticleCount2,
                 "Particle Count 2",
                 SettingDescriptor::Int,
                 1000,
                 limits::particle_count_min,
                 limits::particle_count_max},
                {SettingKey::CollisionModelSphereRadius2,
                 "Sphere Radius 2",
                 SettingDescriptor::Double,
                 1.0,
                 limits::smallest_greater_than_zero,
                 limits::double_max},
                {SettingKey::CollisionModelTotalMass2,
                 "Total Mass 2",
                 SettingDescriptor::Double,
                 1000.0,
                 limits::smallest_greater_than_zero,
                 limits::double_max}};
    }
};
