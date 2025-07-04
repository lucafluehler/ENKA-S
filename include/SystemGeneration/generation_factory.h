#ifndef GENERATION_FACTORY_H_
#define GENERATION_FACTORY_H_

#include <memory>

#include "generator.h"
#include "generation_settings.h"

class GenerationFactory {
public:
    static std::unique_ptr<Generator> create(const GenerationSettings& settings);
};

#endif // GENERATION_FACTORY_H_
