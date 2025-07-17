#pragma once

#include <enkas/data/system.h>

#include <QString>
#include <optional>

#include "core/settings.h"

class FileType;

class INewSimulationView {
public:
    virtual ~INewSimulationView() = default;

    virtual void updatePreview() = 0;
    virtual void processSettings(const std::optional<Settings>& settings) = 0;
    virtual void processInitialSystem(const std::optional<enkas::data::System>& system) = 0;
    virtual void showSimulationProgress() = 0;
    virtual void updateSimulationProgress(double time, double duration) = 0;
    virtual void simulationAborted() = 0;
};
