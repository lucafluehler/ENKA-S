#pragma once

#include <QString>
#include <QVariant>

#include "core/settings/setting_key.h"

namespace limits {
constexpr double zero = 0.0;
constexpr double smallest_greater_than_zero = 1e-15;
constexpr double double_max = 1e10;
constexpr int particle_count_min = 1;
constexpr int particle_count_max = 1'000'000;
constexpr int random_min = 0;
constexpr int random_max = 1'000'000'000;
}  // namespace limits

struct SettingDescriptor {
    SettingKey key;
    QString label;
    enum Type { Double, Int, FilePath, RandomInt } type;
    QVariant defaultValue;
    QVariant min, max;  // only for numerics
};
