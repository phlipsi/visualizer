#version 130
in vec3 v_color;
in vec2 v_texture_coord;

out vec4 o_color;

uniform sampler2D the_texture;

void main() {
    o_color = texture(the_texture, v_texture_coord); // + vec4(v_color, 1);
    //o_color = texture(the_texture, vec2(0, 0)); // + vec4(v_color, 1);
}
