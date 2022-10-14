#version 130
in vec3 i_position;
in vec3 i_color;
in vec2 i_texture_coord;

out vec3 v_color;
out vec2 v_texture_coord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    v_color = i_color;
    v_texture_coord = i_texture_coord;
    gl_Position = projection * view * model * vec4(i_position, 1.0);
    //gl_Position = vec4(i_position, 1.0);
}
