#version 130
in vec3 i_position;
in vec3 i_color;

out vec3 v_color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    v_color = i_color;
    gl_Position = projection * view * model * vec4(i_position, 1.0);
    //gl_Position = vec4(i_position, 1.0);
}
