#version 330

in vec2 frag_texcoord;
in vec4 frag_color;
in vec4 frag_position;
in vec4 frag_normal;
in vec4 frag_tangent;

out vec4 out_color;

void main()
{
    vec2 uv = frag_texcoord;
    out_color = vec4(uv, 0.0, 1.0);
}