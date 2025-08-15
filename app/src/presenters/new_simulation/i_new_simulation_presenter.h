#pragma once

class INewSimulationPresenter {
public:
    /**
     * @brief Called when the tab becomes active, starting the timer for updating the particle
     * rendering.
     */
    virtual void active() = 0;

    /**
     * @brief Called when the tab becomes inactive, stopping the timer for updating the
     * particle rendering.
     */
    virtual void inactive() = 0;

    /**
     * @brief Checks the initial system file and starts parsing it.
     */
    virtual void checkInitialSystemFile() = 0;

    /**
     * @brief Checks the settings file and starts parsing it.
     */
    virtual void checkSettingsFile() = 0;

    /**
     * @brief Starts the simulation using the parsed settings and initial system.
     */
    virtual void startSimulation() = 0;

    /**
     * @brief Aborts the simulation if it is running.
     */
    virtual void abortSimulation() = 0;

    /**
     * @brief Opens the simulation window to display the simulation results.
     */
    virtual void openSimulationWindow() = 0;
};
