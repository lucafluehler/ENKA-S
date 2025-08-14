#pragma once

#include <gmock/gmock.h>

#include "managers/i_simulation_player.h"

class MockSimulationPlayer : public ISimulationPlayer {
public:
    explicit MockSimulationPlayer(QObject* parent = nullptr) : ISimulationPlayer(parent) {}

    MOCK_METHOD(void, run, (std::optional<SystemData>, std::optional<DiagnosticsData>), (override));

    void emitWindowClosed() { emit windowClosed(); }
};
