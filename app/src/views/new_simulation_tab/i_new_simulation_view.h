#pragma once

#include <enkas/data/system.h>

#include <QString>
#include <optional>

#include "core/settings/settings.h"

class FileType;

class INewSimulationView {
public:
    virtual ~INewSimulationView() = default;

    /**
     * @brief Updates the particle rendering of the currently selected generation method.
     */
    virtual void updatePreview() = 0;

    /**
     * @brief Processes the loaded settings and updates the UI accordingly.
     * @param settings The loaded settings, or std::nullopt if loading failed.
     */
    virtual void processSettings(const std::optional<Settings>& settings) = 0;

    /**
     * @brief Processes the initial system data and updates the UI.
     * @param system The loaded system, or std::nullopt if loading failed.
     */
    virtual void processInitialSystem(const std::optional<enkas::data::System>& system) = 0;

    /**
     * @brief Displays the simulation progress UI.
     */
    virtual void showSimulationProgress() = 0;

    /**
     * @brief Updates the simulation progress display.
     * @param time The current simulation time.
     * @param duration The total duration of the simulation.
     */
    virtual void updateSimulationProgress(double time, double duration) = 0;

    /**
     * @brief Handles the simulation abortion process.
     */
    virtual void simulationAborted() = 0;

    /**
     * @brief Gets the path to the initial system file.
     * @return The path as a QString.
     */
    virtual QString getInitialSystemPath() const = 0;

    /**
     * @brief Gets the path to the settings file.
     * @return The path as a QString.
     */
    virtual QString getSettingsPath() const = 0;

    /**
     * @brief Fetches the current simulation settings.
     * @return The current settings as a Settings object.
     */
    virtual Settings fetchSettings() const = 0;
};
