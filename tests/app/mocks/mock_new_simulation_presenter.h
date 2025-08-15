#pragma once

#include <gmock/gmock.h>

#include "presenters/i_new_simulation_presenter.h"

class MockNewSimulationPresenter : public INewSimulationPresenter {
public:
    MOCK_METHOD(void, active, (), (override));
    MOCK_METHOD(void, inactive, (), (override));
    MOCK_METHOD(void, checkInitialSystemFile, (), (override));
    MOCK_METHOD(void, checkSettingsFile, (), (override));
    MOCK_METHOD(void, startSimulation, (), (override));
    MOCK_METHOD(void, abortSimulation, (), (override));
    MOCK_METHOD(void, openSimulationWindow, (), (override));
};
