#pragma once

#include <iosfwd>
#include <vector>

#include <enkas/data/system.h>
#include <enkas/generation/generator.h>

namespace enkas::generation {

class StreamGenerator : public Generator
{
public:
    explicit StreamGenerator(std::istream& input_stream);

    [[nodiscard]] data::System createSystem() override;

private:
    std::istream& stream_;
};

} // namespace enkas::generation
