#version 130
in vec3 i_position;
in vec4 i_color;

out vec4 v_color;

uniform mat4 projection;

void main() {
    v_color = i_color;
    gl_Position = projection * vec4(i_position, 1.0);
    //gl_Position = vec4(i_position, 1.0);
}
