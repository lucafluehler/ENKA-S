#pragma once

#include <enkas/data/system.h>

#include <QString>
#include <QVector>
#include <optional>

#include "core/settings/settings.h"

class ILoadSimulationView {
public:
    virtual ~ILoadSimulationView() = default;

    virtual void updateInitialSystemPreview() = 0;
    virtual void onSettingsParsed(std::optional<Settings> settings) = 0;
    virtual void onInitialSystemParsed(std::optional<enkas::data::System>) = 0;
    virtual void onDiagnosticsSeriesParsed(bool success) = 0;
    virtual QVector<QString> getFilesToCheck() const = 0;
};
