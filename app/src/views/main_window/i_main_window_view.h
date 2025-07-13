#pragma once

class ILoadSimulationView;
class INewSimulationView;

class IMainWindowView {
public:
    virtual ~IMainWindowView() = default;

    virtual void updatePreview() = 0;
    virtual ILoadSimulationView* getLoadSimulationView() = 0;
    virtual INewSimulationView* getNewSimulationView() = 0;
};
