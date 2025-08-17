#pragma once

#include <memory>

#include "application.h"

class CompositionRoot {
public:
    CompositionRoot() = delete;

    static std::unique_ptr<Application> compose();

private:
    static std::unique_ptr<Application::Services> setupServices();
    static void setupLogging(Application::Services& services);
};
