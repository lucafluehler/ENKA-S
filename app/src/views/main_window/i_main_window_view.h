#pragma once

class IMainWindowView {
public:
    virtual ~IMainWindowView() = default;

    virtual void updateHomeScreen() = 0;
};
