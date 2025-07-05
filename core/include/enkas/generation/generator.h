#pragma once

#include <vector>

#include "utils.h"

class Generator
{
public:
    virtual utils::InitialSystem createSystem() = 0;
};
