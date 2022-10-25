#version 330 core
in vec3 position;
in vec3 color;
in float glow;

out vec3 vertex_position;
out vec3 vertex_color;
out float vertex_glow;

uniform mat4 projection;
uniform mat4 view;

void main() {
    vertex_color = color;
    vertex_position = position;
    vertex_glow = glow;
    gl_Position = projection * view * vec4(position, 1.0);
    /*if (gl_VertexID == 0) {
        gl_Position = vec4(-0.5, -0.5, 0.0, 1.0);
        //gl_Position = vec4(position, 1.0);
    } else if (gl_VertexID == 1) {
        gl_Position = vec4(0.5, -0.5, 0.0, 1.0);
        //gl_Position = vec4(position, 1.0);
    } else if (gl_VertexID == 2) {
        //gl_Position = vec4(0.0, 0.5, 0.0, 1.0);
        gl_Position = vec4(position, 1.0);
    }*/
    //gl_Position = vec4(position, 1.0);
}
