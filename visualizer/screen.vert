#version 330 core
in vec2 position;
in vec2 texture_coord;

out vec2 vertex_texture_coord;

uniform mat2 projection;

void main() {
    vertex_texture_coord = texture_coord;
    gl_Position = vec4(projection * position, 0.0, 1.0);
}
