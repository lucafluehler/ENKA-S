#pragma once

#include <array>
#include <optional>
#include <string_view>

namespace enkas::simulation {

enum class SimulationMethod { Euler, Leapfrog, Hermite, HITS, BarnesHutLeapfrog };

constexpr auto SimulationMethodStrings =
    std::to_array<std::pair<SimulationMethod, std::string_view>>(
        {{SimulationMethod::Euler, "Euler"},
         {SimulationMethod::Leapfrog, "Leapfrog"},
         {SimulationMethod::Hermite, "Hermite"},
         {SimulationMethod::HITS, "Hermite Individual Time Steps"},
         {SimulationMethod::BarnesHutLeapfrog, "Barnes-Hut Algorithm (Leapfrog)"}});

[[nodiscard]] constexpr std::string_view toString(SimulationMethod method) {
    for (auto&& [key, value] : SimulationMethodStrings) {
        if (key == method) return value;
    }
    return "<Unknown>";
}

[[nodiscard]] constexpr std::optional<SimulationMethod> toSimulationMethod(
    std::string_view methodStr) {
    for (auto&& [key, value] : SimulationMethodStrings) {
        if (value == methodStr) return key;
    }
    return std::nullopt;
}

}  // namespace enkas::simulation
