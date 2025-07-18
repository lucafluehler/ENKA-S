#version 410 core

// A corner of our base quad (-0.5 to 0.5)
layout (location = 0) in vec2 a_quad_vertex;

// The 3D world position of this specific particle (one per instance)
layout (location = 1) in vec3 a_particle_position;

// Uniforms: constant for all vertices in a draw call
uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform float u_particle_size;

// Data to send to the fragment shader
out float v_distance_to_camera;

void main() {
    // Calculate the particle's center position in camera space
    vec4 camera_space_pos = u_view_matrix * vec4(a_particle_position, 1.0);
    
    // Pass the distance to the fragment shader for fog calculation
    v_distance_to_camera = length(camera_space_pos.xyz);
    
    // Expand the quad vertex by the particle size and add it to the center position.
    // This creates a "billboard" that always faces the camera.
    camera_space_pos.xy += a_quad_vertex * u_particle_size;
    
    // Project the final vertex position to the screen
    gl_Position = u_projection_matrix * camera_space_pos;
}