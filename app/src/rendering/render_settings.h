#pragma once

enum class AnimationStyle
{
    NONE,
    RIGHT,
    LEFT,
    UP,
    DOWN,
    CWISE, // Clockwise
    CNTRCWISE, // Counterclockwise
    TUTTI
};

enum class ColoringMethod
{
    BLACK_FOG,
    WHITE_FOG
};

struct RenderSettings
{
    bool show_center_of_mass = true;
    bool show_center_of_screen = false;
    AnimationStyle animation_style = AnimationStyle::NONE;
    double animation_speed = 1;
    ColoringMethod coloring_method = ColoringMethod::WHITE_FOG;
    double black_fog_param = 5;
    double white_fog_param = 6;
    double particle_size_param = 1;
    int max_fps = 60;
    int fov = 90;
};
