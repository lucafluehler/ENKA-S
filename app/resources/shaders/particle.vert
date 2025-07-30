#version 410 core

layout (location = 0) in vec2 a_quad_vertex;  // The corner of our quad (-0.5 to 0.5)
layout (location = 1) in vec3 a_particle_position;

uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform float u_particle_size;

out vec2 v_quad_coord; 

out float v_distance_to_camera;

void main() {
    vec4 camera_space_pos = u_view_matrix * vec4(a_particle_position, 1.0);
    v_distance_to_camera = length(camera_space_pos.xyz);
    
    camera_space_pos.xy += a_quad_vertex * u_particle_size;
    
    v_quad_coord = a_quad_vertex;

    gl_Position = u_projection_matrix * camera_space_pos;
}