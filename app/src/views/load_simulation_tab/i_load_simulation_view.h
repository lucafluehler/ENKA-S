#pragma once

#include <enkas/data/system.h>

#include <QString>
#include <QVector>
#include <optional>

#include "core/settings/settings.h"

class ILoadSimulationView {
public:
    virtual ~ILoadSimulationView() = default;

    /**
     * @brief Updates the particle rendering of the loaded initial system.
     */
    virtual void updateInitialSystemPreview() = 0;

    /**
     * @brief Called when the settings file is parsed.
     * @param settings The parsed settings, or std::nullopt if parsing failed.
     */
    virtual void onSettingsParsed(std::optional<Settings> settings) = 0;

    /**
     * @brief Called when the initial system is parsed.
     * @param system The parsed system, or std::nullopt if parsing failed.
     */
    virtual void onInitialSystemParsed(std::optional<enkas::data::System> system) = 0;

    /**
     * @brief Called when the diagnostics series is parsed.
     * @param success True if parsing was successful, false otherwise.
     */
    virtual void onDiagnosticsSeriesParsed(bool success) = 0;

    /**
     * @brief Retrieves the file paths to check for validity.
     * @return A vector of file paths to check.
     */
    virtual QVector<QString> getFilesToCheck() const = 0;
};
