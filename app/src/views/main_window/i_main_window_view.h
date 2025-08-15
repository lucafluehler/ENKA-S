#pragma once

class IMainWindowView {
public:
    virtual ~IMainWindowView() = default;

    /**
     * @brief Updates the home screen particle rendering if the home tab is active.
     */
    virtual void updateHomeScreen() = 0;

    /**
     * @brief Gets the index of the currently active tab.
     * @return The index of the current tab.
     */
    virtual int getCurrentTabIndex() const = 0;
};
