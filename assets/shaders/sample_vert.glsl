#version 330
// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec3 vertexTangent;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;
uniform vec3 lightPos;
uniform vec4 difColor;

// Output vertex attributes (to fragment shader)
out vec3 frag_position;
out vec2 frag_texcoord;
out vec4 frag_color;
out vec3 frag_normal;
out vec3 frag_tangent;
out mat3 TBN;

const float normalOffset = 0.1;

void main()
{
    vec3 vertex_binormal = cross(vertexNormal, vertexTangent);
    mat3 normal_matrix = transpose(inverse(mat3(matModel)));
    frag_position = vec3(matModel*vec4(vertexPosition, 1.0f));
    frag_texcoord = vertexTexCoord*2.0;
    frag_normal = normalize(normal_matrix*vertexNormal);
    frag_tangent = normalize(normal_matrix*vertexTangent);
    frag_tangent = normalize(frag_tangent - dot(frag_tangent, frag_normal)*frag_normal);
    vec3 frag_binormal = normalize(normal_matrix*vertex_binormal);
    frag_binormal = cross(frag_normal, frag_tangent);
    TBN = transpose(mat3(frag_tangent, frag_binormal, frag_normal));
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}
