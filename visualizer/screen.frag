#version 330 core
in vec2 vertex_texture_coord;

uniform sampler2D screen_texture;
uniform sampler2D glow_texture;

out vec4 FragColor;

void main() {
    //vec3 color = vec3(0.0); // texture(screen_texture, vertex_texture_coord).rgb;
    vec3 color = texture(screen_texture, vertex_texture_coord).rgb;
    vec3 glow = texture(glow_texture, vertex_texture_coord).rgb;
    color += glow;
    //FragColor = vec4(color, 1.0);
    vec3 result = vec3(1.0) - exp(-color);
    //result = pow(result, vec3(1.0 / 1.0));
    FragColor = vec4(result, 1.0);
}
