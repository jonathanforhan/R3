#version 460

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in ivec4 a_JointIDs;
layout (location = 4) in vec4 a_Weights;

layout(binding = 0) uniform ModelViewProjection {
    mat4 u_View;
    mat4 u_Proj;
};

layout (push_constant) uniform VertexPushConstants {
    mat4 c_Model;
};

void main() {
    vec3 position = vec3(c_Model * vec4(a_Position, 1.0));
    gl_Position = u_Proj * u_View * vec4(position, 1.0);
}
