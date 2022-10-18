#version 330 core
out vec4 FragColor;

in vec2 v_texture_coord;

uniform sampler2D screen_texture;
uniform float alpha;

const float offset = 1.0 / 300.0;

const vec2 offsets[] = vec2[](
    vec2(-offset, -offset),
    vec2(    0.0, -offset),
    vec2( offset, -offset),

    vec2(-offset,     0.0),
    vec2(    0.0,     0.0),
    vec2( offset,     0.0),

    vec2(-offset,  offset),
    vec2(    0.0,  offset),
    vec2( offset,  offset)
);

/*const float kernel[9] = float[](
    1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
    2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
    1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
);*/

const float kernel[9] = float[](
    -1.0, -1.0, -1.0,
    -1.0,  9.0, -1.0,
    -1.0, -1.0, -1.0
);

float triangle_wave(float x) {
    return 2.0 * abs(x - floor(x + 0.5));
}

vec2 kaleidoscope(vec2 pos) {
    pos = 2 * pos - vec2(1.0, 1.0);
    float r = length(pos);
    float beta = atan(pos.x, pos.y);
    beta = alpha * triangle_wave(beta / (2.0 * alpha));
    return 0.5 * vec2(r * cos(beta) + 1.0, r * sin(beta) + 1.0);
}


void main() {
    vec3 offset_colors[9];
    for (int i = 0; i < 9; i++) {
        offset_colors[i] = vec3(texture(screen_texture, kaleidoscope(v_texture_coord + offsets[i])));
    }
    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        color += offset_colors[i] * kernel[i];
    }
    FragColor = vec4(color, 1.0);
}
