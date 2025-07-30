#version 410 core

in vec2 v_quad_coord;

in float v_distance_to_camera;

uniform float u_fog_mu;
uniform float u_camera_target_distance;
uniform int u_coloring_method; 

out vec4 frag_color;

void main() {
    if (dot(v_quad_coord, v_quad_coord) > 0.25) {
        discard; // Discard fragment if it's outside the circle
    }
    
    float c = 0.0;
    float epsilon = 0.00001; // A small number to prevent division by zero

    if (u_coloring_method == 0) { // BLACK_FOG
        float safe_denom = (u_camera_target_distance * 1.3) + epsilon;
        float ratio = v_distance_to_camera / safe_denom;
        c = 1.0 / (1.0 + exp((ratio - 1.0) * u_fog_mu));
    } else { // WHITE_FOG
        float safe_denom = u_camera_target_distance + epsilon;
        float ratio = v_distance_to_camera / safe_denom;
        c = 1.0 - (1.0 / (1.0 + exp((ratio - 1.0) * u_fog_mu)));
    }
        
    frag_color = vec4(c, pow(c, 6.0), pow(c, 8.0), 1.0);
}