#pragma once

#include <QString>
#include <QVariant>

#include "core/settings/setting_key.h"

struct SettingDescriptor {
    SettingKey key;
    QString label;
    enum Type { Double, Int, FilePath, RandomInt } type;
    QVariant defaultValue;
    QVariant min, max;  // only for numerics
};
