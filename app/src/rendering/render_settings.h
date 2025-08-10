#pragma once

#include "rendering/is_darkmode.h"

/**
 * @brief Defines animation styles for particle rendering.
 * These styles determine how the system rotates relative to the camera. For example, Right means
 * that the system appears rotating to the right from the camera's perspective.
 */
enum class AnimationStyle {
    None,
    Right,
    Left,
    Up,
    Down,
    Clockwise,
    Counterclockwise,
    Tutti  // Up, Right, Counterclockwise simultaneously
};

enum class ColoringMethod {
    // Dark background. Far away particles appear black, close ones appear white. Red in the center.
    BlackFog,

    // Light background. Far away particles appear white, close ones appear black. Red in the
    // center.
    WhiteFog
};

struct RenderSettings {
    // Toggles
    bool show_center_of_mass = false;
    bool show_center_of_screen = false;

    // Animation
    AnimationStyle animation_style = AnimationStyle::None;
    double animation_speed = 1;

    // Coloring
    ColoringMethod coloring_method;
    double black_fog_param = 5;
    double white_fog_param = 6;

    // Rendering
    double particle_size_param = 100;
    int fov = 90;

    RenderSettings()
        : coloring_method(isDarkMode() ? ColoringMethod::BlackFog : ColoringMethod::WhiteFog) {}
};
