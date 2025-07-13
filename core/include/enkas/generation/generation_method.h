#pragma once

#include <array>
#include <optional>
#include <string_view>

namespace enkas::generation {

enum class GenerationMethod {
    NormalSphere,
    UniformCube,
    UniformSphere,
    PlummerSphere,
    SpiralGalaxy,
    CollisionModel
};

constexpr auto GenerationMethodStrings =
    std::to_array<std::pair<GenerationMethod, std::string_view>>(
        {{GenerationMethod::NormalSphere, "Normal Sphere"},
         {GenerationMethod::UniformCube, "Uniform Cube"},
         {GenerationMethod::UniformSphere, "Uniform Sphere"},
         {GenerationMethod::PlummerSphere, "Plummer Model"},
         {GenerationMethod::SpiralGalaxy, "Spiral Galaxy"},
         {GenerationMethod::CollisionModel, "Collision Model"}});

[[nodiscard]] constexpr std::string_view toString(GenerationMethod method) {
    for (auto&& [key, value] : GenerationMethodStrings) {
        if (key == method) return value;
    }
    return "<Unknown>";
}

[[nodiscard]] constexpr std::optional<GenerationMethod> toGenerationMethod(
    std::string_view methodStr) {
    for (auto&& [key, value] : GenerationMethodStrings) {
        if (value == methodStr) return key;
    }
    return std::nullopt;
}

}  // namespace enkas::generation
