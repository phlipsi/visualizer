#version 330 core
in vec2 vertex_texture_coord;

uniform sampler2D screen_texture;

out vec4 color;

void main() {
    color = texture(screen_texture, vertex_texture_coord);
}
