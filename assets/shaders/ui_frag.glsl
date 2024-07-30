#version 330

uniform sampler2D texture0;

in vec2 frag_texcoord;
in vec4 frag_vert_color;

out vec4 final_color;

void main() {
    vec2 uv = frag_texcoord;
    vec4 texture_color = texture2D(texture0, frag_texcoord);
    final_color = vec4(texture_color.xyz, 1);//vec4(uv.x , uv.y, 0., max(max(texture_color.x, texture_color.y), texture_color.z));
}