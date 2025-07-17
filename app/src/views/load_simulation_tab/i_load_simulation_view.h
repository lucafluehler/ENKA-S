#pragma once

#include <enkas/data/system.h>

#include <optional>

class ILoadSimulationView {
public:
    virtual ~ILoadSimulationView() = default;

    virtual void updatePreview() = 0;
    virtual void onSettingsParsed(bool success) = 0;
    virtual void onInitialSystemParsed(std::optional<enkas::data::System>) = 0;
    virtual void onDiagnosticsSeriesParsed(bool success) = 0;
};
