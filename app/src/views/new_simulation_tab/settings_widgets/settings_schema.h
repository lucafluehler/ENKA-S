#pragma once

#include <QString>
#include <QVector>

#include "setting_descriptor.h"

class SettingsSchema {
public:
    virtual ~SettingsSchema() = default;
    virtual QString name() const = 0;
    virtual QVector<SettingDescriptor> settingsSchema() const = 0;
};
