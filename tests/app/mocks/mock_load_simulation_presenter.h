#pragma once

#include <gmock/gmock.h>

#include "presenters/i_load_simulation_presenter.h"

class MockLoadSimulationPresenter : public ILoadSimulationPresenter {
public:
    MOCK_METHOD(void, checkFiles, (), (override));
    MOCK_METHOD(void, active, (), (override));
    MOCK_METHOD(void, inactive, (), (override));
    MOCK_METHOD(void, playSimulation, (), (override));
    MOCK_METHOD(void, endSimulationPlayback, (), (override));
};
