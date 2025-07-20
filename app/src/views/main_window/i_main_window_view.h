#pragma once

class IMainWindowView {
public:
    virtual ~IMainWindowView() = default;

    /**
     * @brief Updates the home screen particle rendering if the home tab is active.
     */
    virtual void updateHomeScreen() = 0;
};
