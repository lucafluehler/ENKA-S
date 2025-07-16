#pragma once

#include <string_view>

namespace file_names {
inline constexpr std::string_view settings = "settings.csv";
inline constexpr std::string_view system = "system.csv";
inline constexpr std::string_view diagnostics = "diagnostics.csv";
}  // namespace file_names

namespace folder_names {
inline constexpr std::string_view output = "output";
}  // namespace folder_names
