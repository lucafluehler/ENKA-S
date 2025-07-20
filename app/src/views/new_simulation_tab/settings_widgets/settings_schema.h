#pragma once

#include <QString>
#include <QVector>

#include "setting_descriptor.h"

class SettingsSchema {
public:
    virtual ~SettingsSchema() = default;

    /**
     * @brief Returns the name of the settings schema.
     * @return QString representing the name of the schema.
     */
    virtual QString name() const = 0;

    /**
     * @brief Returns the settings schema as a vector of SettingDescriptor.
     * @return QVector<SettingDescriptor> representing the settings schema.
     */
    virtual QVector<SettingDescriptor> settingsSchema() const = 0;
};
