#version 330

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec2 vert_texcoord;
layout(location = 2) in vec4 vert_color;

out vec2 frag_texcoord;
out vec4 frag_vert_color;

void main() {
    frag_texcoord = vert_texcoord;
    frag_vert_color = vert_color;
    gl_Position = vec4(vert_position, 1.0);
}
