#pragma once

#include <array>
#include <optional>
#include <string_view>

namespace enkas::generation {

enum class Method {
    NormalSphere,
    UniformCube,
    UniformSphere,
    PlummerSphere,
    SpiralGalaxy,
    CollisionModel
};

constexpr auto GenerationMethodStrings = std::to_array<std::pair<Method, std::string_view>>(
    {{Method::NormalSphere, "Normal Sphere"},
     {Method::UniformCube, "Uniform Cube"},
     {Method::UniformSphere, "Uniform Sphere"},
     {Method::PlummerSphere, "Plummer Model"},
     {Method::SpiralGalaxy, "Spiral Galaxy"},
     {Method::CollisionModel, "Collision Model"}});

[[nodiscard]] constexpr std::string_view methodToString(Method method) {
    for (auto&& [key, value] : GenerationMethodStrings) {
        if (key == method) return value;
    }
    return "<Unknown>";
}

[[nodiscard]] constexpr std::optional<Method> stringToMethod(std::string_view methodStr) {
    for (auto&& [key, value] : GenerationMethodStrings) {
        if (value == methodStr) return key;
    }
    return std::nullopt;
}

}  // namespace enkas::generation
