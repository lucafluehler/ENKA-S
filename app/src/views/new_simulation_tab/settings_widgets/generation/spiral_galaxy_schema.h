#pragma once

#include <QString>

#include "views/new_simulation_tab/settings_widgets/setting_descriptor.h"
#include "views/new_simulation_tab/settings_widgets/settings_schema.h"

class SpiralGalaxySchema : public SettingsSchema {
public:
    QString name() const override { return "Spiral Galaxy"; }
    QVector<SettingDescriptor> settingsSchema() const override {
        return {
            {SettingKey::SpiralGalaxySeed,
             "Seed",
             SettingDescriptor::RandomInt,
             {},
             limits::random_min,
             limits::random_max},
            {SettingKey::SpiralGalaxyParticleCount,
             "Particle Count",
             SettingDescriptor::Int,
             1000,
             limits::particle_count_min,
             limits::particle_count_max},
            {SettingKey::SpiralGalaxyNumArms, "Number of Arms", SettingDescriptor::Int, 2, 2, 10},
            {SettingKey::SpiralGalaxyRadius,
             "Radius",
             SettingDescriptor::Double,
             4.0,
             limits::smallest_greater_than_zero,
             limits::double_max},
            {SettingKey::SpiralGalaxyTotalMass,
             "Total Mass",
             SettingDescriptor::Double,
             10.0,
             limits::smallest_greater_than_zero,
             limits::double_max},
            {SettingKey::SpiralGalaxyTwist,
             "Twist",
             SettingDescriptor::Double,
             2.4,
             limits::smallest_greater_than_zero,
             limits::double_max},
            {SettingKey::SpiralGalaxyBlackHoleMass,
             "Black Hole Mass",
             SettingDescriptor::Double,
             10000.0,
             limits::smallest_greater_than_zero,
             limits::double_max}};
    }
};
