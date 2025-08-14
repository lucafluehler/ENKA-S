#pragma once

#include <gmock/gmock.h>

#include "views/new_simulation_tab/i_new_simulation_view.h"

class MockNewSimulationView : public INewSimulationView {
public:
    MOCK_METHOD(void, updatePreview, (), (override));
    MOCK_METHOD(void, processSettings, (const std::optional<Settings>&), (override));
    MOCK_METHOD(void,
                processInitialSystem,
                (const std::optional<enkas::data::System>&),
                (override));
    MOCK_METHOD(void, showSimulationProgress, (), (override));
    MOCK_METHOD(void, updateSimulationProgress, (double time, double duration), (override));
    MOCK_METHOD(void, simulationAborted, (), (override));
    MOCK_METHOD(QString, getInitialSystemPath, (), (const, override));
    MOCK_METHOD(QString, getSettingsPath, (), (const, override));
    MOCK_METHOD(Settings, fetchSettings, (), (const, override));
};
