#pragma once

#include <memory>

#include "generator.h"
#include "generation_settings.h"

class GenerationFactory {
public:
    static std::unique_ptr<Generator> create(const GenerationSettings& settings);
};
