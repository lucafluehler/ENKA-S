#pragma once

class ILoadSimulationPresenter {
public:
    virtual ~ILoadSimulationPresenter() = default;

    /**
     * @brief Called when the view requests to check user files.
     */
    virtual void checkFiles() = 0;

    /**
     * @brief Called when the tab becomes active.
     * Starts the timer for updating the particle rendering.
     */
    virtual void active() = 0;

    /**
     * @brief Called when the tab becomes inactive.
     * Stops the timer for updating the particle rendering.
     */
    virtual void inactive() = 0;

    /**
     * @brief Starts the simulation playback with the loaded data.
     */
    virtual void playSimulation() = 0;

    /**
     * @brief Ends the simulation playback.
     */
    virtual void endSimulationPlayback() = 0;
};
