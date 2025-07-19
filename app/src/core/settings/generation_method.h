#pragma once

#include <array>
#include <stdexcept>
#include <string>
#include <string_view>

enum class GenerationMethod {
    NormalSphere,
    UniformCube,
    UniformSphere,
    PlummerSphere,
    SpiralGalaxy,
    CollisionModel,
    File,
};

constexpr auto GenerationMethodStrings =
    std::to_array<std::pair<GenerationMethod, std::string_view>>(
        {{GenerationMethod::File, "File"},
         {GenerationMethod::NormalSphere, "NormalSphere"},
         {GenerationMethod::UniformCube, "UniformCube"},
         {GenerationMethod::UniformSphere, "UniformSphere"},
         {GenerationMethod::PlummerSphere, "PlummerSphere"},
         {GenerationMethod::SpiralGalaxy, "SpiralGalaxy"},
         {GenerationMethod::CollisionModel, "CollisionModel"}});

[[nodiscard]] constexpr GenerationMethod stringToGenerationMethod(std::string_view s) {
    for (auto&& [key, val] : GenerationMethodStrings) {
        if (val == s) return key;
    }
    throw std::out_of_range("Unknown GenerationMethod: \"" + std::string(s) + "\"");
}

[[nodiscard]] constexpr std::string_view generationMethodToString(GenerationMethod method) {
    for (auto&& [key, val] : GenerationMethodStrings) {
        if (key == method) return val;
    }
    throw std::out_of_range("Unhandled GenerationMethod enum value");
}
