#version 460

#define MAX_JOINTS 128
#define MAX_JOINT_INFLUENCE 4

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Bitanget;
layout (location = 4) in vec2 a_TexCoords;
layout (location = 5) in ivec4 a_JointIDs;
layout (location = 6) in vec4 a_Weights;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoords;

layout(set = 0, binding = 0) uniform MVP {
    mat4 u_Model;
    mat4 u_View;
    mat4 u_Proj;
};

void main() {
    v_Position = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    v_TexCoords = a_TexCoords;

    gl_Position = u_Proj * u_View * vec4(v_Position, 1.0);
}