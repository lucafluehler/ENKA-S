// In a string literal or a file
#version 410 core

in float v_distance_to_camera;

// Uniforms from the C++ side
uniform float u_fog_mu;
uniform float u_camera_target_distance;
uniform int u_coloring_method; // 0 for black fog, 1 for white fog

out vec4 frag_color;

void main() {
    // Make the square a circle by discarding pixels outside a radius of 0.5
    // `gl_PointCoord` is an input variable for point sprites, but we can fake it.
    // A simpler way for quads is to use a varying tex coord, but for now this works:
    vec2 coord = gl_PointCoord - vec2(0.5);
    if (dot(coord, coord) > 0.25) {
        discard;
    }
    
    float c = 0.0;
    if (u_coloring_method == 0) { // BLACK_FOG
        float ratio = v_distance_to_camera / (u_camera_target_distance * 1.3);
        c = 1.0 / (1.0 + exp((ratio - 1.0) * u_fog_mu));
    } else { // WHITE_FOG
        float ratio = v_distance_to_camera / u_camera_target_distance;
        c = 1.0 - (1.0 / (1.0 + exp((ratio - 1.0) * u_fog_mu)));
    }
    
    frag_color = vec4(c, pow(c, 6.0), pow(c, 8.0), 1.0);
}