#version 410 core
layout (location = 0) in vec2 a_pos; // Unit cross vertex
uniform vec2 u_center; // Center in Normalized Device Coords
uniform vec2 u_size;   // Size in NDC, pre-adjusted for aspect ratio

void main() {
    gl_Position = vec4(a_pos * u_size + u_center, 0.0, 1.0);
}