#pragma once

#include <string>
#include <vector>

namespace file_names {
inline constexpr char settings[] = "settings.json";
inline constexpr char system[] = "system.csv";
inline constexpr char diagnostics[] = "diagnostics.csv";
}  // namespace file_names

namespace csv_headers {
inline const std::vector<std::string> system = {
    "time", "pos_x", "pos_y", "pos_z", "vel_x", "vel_y", "vel_z", "mass"};

inline const std::vector<std::string> diagnostics = {"time",
                                                     "e_kin",
                                                     "e_pot",
                                                     "L_tot",
                                                     "com_pos_x",
                                                     "com_pos_y",
                                                     "com_pos_z",
                                                     "com_vel_x",
                                                     "com_vel_y",
                                                     "com_vel_z",
                                                     "r_vir",
                                                     "ms_vel",
                                                     "t_cr"};
}  // namespace csv_headers
