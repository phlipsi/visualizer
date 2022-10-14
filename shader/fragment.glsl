#version 130
in vec2 v_texture_coord;

out vec4 o_color;

uniform sampler2D the_texture;

void main() {
    o_color = texture(the_texture, v_texture_coord);
}
