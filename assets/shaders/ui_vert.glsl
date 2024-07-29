#version 330

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec2 vert_texcoord;

out vec2 frag_texcoord;

void main() {
    frag_texcoord = vert_texcoord;
    gl_Position = vec4(vert_position, 1.0);
}