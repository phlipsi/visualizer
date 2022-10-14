#version 130
in vec3 v_normal;
in vec2 v_texture_coord;
in vec3 v_fragment_pos;

out vec4 o_color;

uniform sampler2D the_texture;
uniform vec3 light;

void main() {
    float diff = max(dot(normalize(v_normal), normalize(light - v_fragment_pos)), 0);
    o_color = (diff + 0.1) * texture(the_texture, v_texture_coord);
}
