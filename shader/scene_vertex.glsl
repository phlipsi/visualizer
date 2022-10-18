#version 330 core
in vec3 position;
in vec3 normal;
in vec3 color;

out vec3 vertex_position;
out vec3 vertex_normal;
out vec3 vertex_color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    vertex_normal = vec3(model * vec4(normal, 0.0));
    vertex_color = color;
    vec4 world = model * vec4(position, 1.0);
    vertex_position = vec3(world);
    gl_Position = /*projection * view * world*/ projection * view * model * vec4(position, 1.0);
}
