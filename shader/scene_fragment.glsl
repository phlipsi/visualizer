#version 330 core
in vec3 vertex_position;
in vec3 vertex_normal;
in vec3 vertex_color;

out vec4 color;

uniform vec3 light;

void main() {
    float diff = max(dot(normalize(vertex_normal), normalize(light - vertex_position)), 0);
    color = vec4((diff + 0.1) * vertex_color, 1.0);
}
