#pragma once

#include <enkas/generation/generation_settings.h>

namespace enkas::generation {
class Generator;
}  // namespace enkas::generation

#include <memory>

namespace enkas::generation {

class Factory {
public:
    /**
     * @brief Creates a generator based on a settings configuration object.
     * @param config The configuration object containing the specific settings and seed.
     * @return A unique_ptr to the created generator, or nullptr if config is invalid.
     */
    [[nodiscard]] static std::unique_ptr<Generator> create(const Settings& settings);
};

}  // namespace enkas::generation
