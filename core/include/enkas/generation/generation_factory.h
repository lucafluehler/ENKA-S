#pragma once

#include <memory>
#include <iosfwd>

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
    /**
     * @brief Creates a generator based on a settings configuration object.
     * @param config The configuration object containing the specific settings and seed.
     * @return A unique_ptr to the created generator, or nullptr if config is invalid.
     */
    [[nodiscard]] static std::unique_ptr<Generator> create(const GenerationConfig& config);

    /**
     * @brief Creates a generator that reads its data from a stream.
     * @param stream The input stream to read particle data from.
     * @return A unique_ptr to the created generator, or nullptr if the stream is invalid.
     */
    [[nodiscard]] static std::unique_ptr<Generator> create(std::istream& stream);
};

} // namespace generation
} // namespace enkas