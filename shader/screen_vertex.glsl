#version 330 core
in vec2 i_position;
in vec2 i_texture_coord;

out vec2 v_texture_coord;

uniform mat2 projection;

void main() {
    v_texture_coord = i_texture_coord;
    gl_Position = vec4(projection * i_position, 0.0, 1.0);
}
