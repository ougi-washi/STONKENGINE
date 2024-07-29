#version 330

uniform sampler2D texture0;

in vec2 frag_texcoord;
out vec4 final_color;

void main() {
    vec2 uv = frag_texcoord;
    vec4 texture_color = texture(texture0, uv);
    final_color = vec4(texture_color.xy , 0.0, 1.0);
}