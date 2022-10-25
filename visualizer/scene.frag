#version 330 core
in vec3 vertex_position;
in vec3 vertex_color;
in float vertex_glow;

out vec4 color;

uniform vec3 light;

void main() {
    float diff = max(normalize(light - vertex_position).z, 0);
    color = vec4((diff + vertex_glow) * vertex_color, 1.0);
    //color = vec4(1.0, 1.0, 1.0, 1.0);
    //color = vec4(vertex_color, 1.0);
}