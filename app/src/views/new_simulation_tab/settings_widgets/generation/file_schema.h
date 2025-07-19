#pragma once

#include <QString>

#include "views/new_simulation_tab/settings_widgets/setting_descriptor.h"
#include "views/new_simulation_tab/settings_widgets/settings_schema.h"

class FileSchema : public SettingsSchema {
public:
    QString name() const override { return "Import from File"; }
    QVector<SettingDescriptor> settingsSchema() const override {
        return {
            {SettingKey::FilePath, "File Path", SettingDescriptor::FilePath, {}, {}, {}},
        };
    }
};
