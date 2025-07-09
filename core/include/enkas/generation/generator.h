#pragma once

#include <enkas/data/system.h>

namespace enkas::generation {

class Generator {
public:
    virtual ~Generator() = default;

    /**
     * @brief Creates a new system based on the generator's logic.
     * @return An System object containing the generated system in units
     *         of pc, solar mass, and km/s.
     */
    [[nodiscard]] virtual data::System createSystem() = 0;
};

}  // namespace enkas::generation
