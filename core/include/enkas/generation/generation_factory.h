#pragma once

#include <memory>

// Forward declarations
namespace enkas {
namespace generation {

class Generator;
struct GenerationConfig;

} // namespace generation
} // namespace enkas

namespace enkas {
namespace generation {

class GenerationFactory {
public:
    [[nodiscard]] static std::unique_ptr<Generator> create(const GenerationConfig& config);
};

} // namespace generation
} // namespace enkas