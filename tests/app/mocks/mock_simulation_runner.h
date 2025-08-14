#pragma once

#include <gmock/gmock.h>

#include "core/settings/settings.h"
#include "managers/i_simulation_runner.h"

class MockSimulationRunner : public ISimulationRunner {
public:
    explicit MockSimulationRunner(const Settings& settings, QObject* parent = nullptr)
        : ISimulationRunner(parent) {}

    MOCK_METHOD(void, startSimulationProcedure, (), (override));
    MOCK_METHOD(void, openSimulationWindow, (), (override));
    MOCK_METHOD(double, getTime, (), (const, override));
    MOCK_METHOD(double, getDuration, (), (const, override));

    void emitGenerationCompleted() { emit generationCompleted(); }

    void emitInitializationCompleted() { emit initializationCompleted(); }
};
