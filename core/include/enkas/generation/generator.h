#pragma once

#include <vector>

#include <enkas/data/particle.h>

namespace enkas {
namespace generation {

class Generator
{
public:
     virtual ~Generator() = default;
    [[nodiscard]] virtual InitialSystem createSystem() = 0;
};

} // namespace generation
} // namespace enkas
