#version 330 core
in vec2 vertex_texture_coord;

uniform sampler2D input_texture;
uniform bool horizontal;

const float weights[5] =  float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
//const float weights[8] =  float[] (0.13298076, 0.125794409, 0.106482669, 0.080656908, 0.054670025, 0.033159046, 0.017996989, 0.00874063);

out vec4 color;

void main() {
    vec2 texel_size = 1.0 / textureSize(input_texture, 0);
    vec3 result = texture(input_texture, vertex_texture_coord).rgb * weights[0];
    if (horizontal) {
        for (int i = 1; i < 5; ++i) {
            result += texture(input_texture, vertex_texture_coord + vec2(texel_size.x * i, 0.0)).rgb * weights[i];
            result += texture(input_texture, vertex_texture_coord - vec2(texel_size.x * i, 0.0)).rgb * weights[i];
        }
    } else {
        for (int i = 1; i < 5; ++i) {
            result += texture(input_texture, vertex_texture_coord + vec2(0.0, texel_size.y * i)).rgb * weights[i];
            result += texture(input_texture, vertex_texture_coord - vec2(0.0, texel_size.y * i)).rgb * weights[i];
        }
    }
    color = vec4(result, 1.0);
}
