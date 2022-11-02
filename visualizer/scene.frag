#version 330 core
in vec3 vertex_position;
in vec3 vertex_color;
in float vertex_glow;

out vec4 color;
out vec4 glow;

uniform vec3 light;

void main() {
    float diff = max(normalize(light - vertex_position).z, 0);
    color = vec4((vertex_glow + 0.05) * diff * vertex_color, 1.0);
    glow = vec4(vertex_glow * vertex_color, 1.0);
    //color = vec4(1.0, 1.0, 1.0, 1.0);
    //color = vec4(vertex_color, 1.0);
}
