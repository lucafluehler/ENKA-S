#pragma once

#include <iosfwd>
#include <vector>

#include <enkas/data/initial_system.h>
#include <enkas/generation/generator.h>

namespace enkas {
namespace generation {

class StreamGenerator : public Generator
{
public:
    explicit StreamGenerator(std::istream& input_stream);

    [[nodiscard]] data::InitialSystem createSystem() override;

private:
    std::istream& stream;
};

} // namespace generation
} // namespace enkas
