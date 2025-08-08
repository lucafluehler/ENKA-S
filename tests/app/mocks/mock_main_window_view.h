#pragma once

#include <gmock/gmock.h>

#include "views/main_window/i_main_window_view.h"

class MockMainWindowView : public IMainWindowView {
public:
    MOCK_METHOD(void, updateHomeScreen, (), (override));
};
