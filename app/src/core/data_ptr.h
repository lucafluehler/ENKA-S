#pragma once

#include <enkas/data/diagnostics.h>
#include <enkas/data/system.h>

#include <memory>

#include "settings.h"

struct DataPtr {
    std::shared_ptr<Settings> settings = nullptr;
    std::shared_ptr<enkas::data::System> system = nullptr;
    std::shared_ptr<enkas::data::Diagnostics> diagnostics = nullptr;
};
