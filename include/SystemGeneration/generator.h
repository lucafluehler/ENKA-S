#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <vector>

#include "utils.h"

class Generator
{
public:
    virtual utils::InitialSystem createSystem() = 0;
};

#endif // GENERATOR_H_
