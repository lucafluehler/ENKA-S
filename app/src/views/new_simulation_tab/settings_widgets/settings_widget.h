#pragma once

#include "core/settings/settings.h"

class SettingsWidget {
public:
    /**
     * @brief Retrieves the current settings from the panel.
     * @return A Settings object containing the current settings.
     */
    virtual Settings getSettings() const;

    /**
     * @brief Sets the settings of the panel.
     * @param settings The Settings object to set.
     */
    virtual void setSettings(const Settings& settings);

    /**
     * @brief Sets the default settings for the panel. These are applied when resetting the panel.
     * @param settings The Settings object to set as default.
     */
    virtual void setDefaultSettings(const Settings& settings);

    /**
     * @brief Resets the settings of the panel to the default values.
     */
    virtual void resetSettings();
};
