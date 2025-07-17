#pragma once

#include <array>
#include <stdexcept>
#include <string>
#include <string_view>

enum class SimulationMethod { Euler, Leapfrog, Hermite, Hits, BarnesHutLeapfrog };

constexpr auto SimulationMethodStrings =
    std::to_array<std::pair<SimulationMethod, std::string_view>>(
        {{SimulationMethod::Euler, "Euler"},
         {SimulationMethod::Leapfrog, "Leapfrog"},
         {SimulationMethod::Hermite, "Hermite"},
         {SimulationMethod::Hits, "Hits"},
         {SimulationMethod::BarnesHutLeapfrog, "BarnesHutLeapfrog"}});

[[nodiscard]] constexpr SimulationMethod stringToSimulationMethod(std::string_view s) {
    for (auto&& [key, val] : SimulationMethodStrings) {
        if (val == s) return key;
    }
    throw std::out_of_range("Unknown SimulationMethod: \"" + std::string(s) + "\"");
}

[[nodiscard]] constexpr std::string_view simulationMethodToString(SimulationMethod method) {
    for (auto&& [key, val] : SimulationMethodStrings) {
        if (key == method) return val;
    }
    throw std::out_of_range("Unhandled SimulationMethod enum value");
}
