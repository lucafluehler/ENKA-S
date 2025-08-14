#pragma once

#include <gmock/gmock.h>

#include "views/load_simulation_tab/i_load_simulation_view.h"

class MockLoadSimulationView : public ILoadSimulationView {
public:
    MOCK_METHOD(void, updateInitialSystemPreview, (), (override));
    MOCK_METHOD(void, onSettingsParsed, (std::optional<Settings>), (override));
    MOCK_METHOD(void, onInitialSystemParsed, (std::optional<enkas::data::System>), (override));
    MOCK_METHOD(void, onDiagnosticsSeriesParsed, (bool), (override));
    MOCK_METHOD(QVector<QString>, getFilesToCheck, (), (const, override));
};
