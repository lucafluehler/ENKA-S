#pragma once

#include <array>
#include <optional>
#include <string_view>

namespace enkas::simulation {

enum class Method { Euler, Leapfrog, Hermite, HITS, BarnesHutLeapfrog };

constexpr auto SimulationMethodStrings = std::to_array<std::pair<Method, std::string_view>>(
    {{Method::Euler, "Euler"},
     {Method::Leapfrog, "Leapfrog"},
     {Method::Hermite, "Hermite"},
     {Method::HITS, "Hermite Individual Time Steps"},
     {Method::BarnesHutLeapfrog, "Barnes-Hut Algorithm (Leapfrog)"}});

[[nodiscard]] constexpr std::string_view methodToString(Method method) {
    for (auto&& [key, value] : SimulationMethodStrings) {
        if (key == method) return value;
    }
    return "<Unknown>";
}

[[nodiscard]] constexpr std::optional<Method> stringToMethod(std::string_view methodStr) {
    for (auto&& [key, value] : SimulationMethodStrings) {
        if (value == methodStr) return key;
    }
    return std::nullopt;
}

}  // namespace enkas::simulation
