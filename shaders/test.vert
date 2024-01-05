#version 460
#extension GL_GOOGLE_include_directive : require

#define MAX_BONES 128
#define MAX_BONE_INFLUENCE 4

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Bitanget;
layout (location = 4) in vec2 a_TexCoords;
layout (location = 5) in ivec4 a_BoneIDs;
layout (location = 6) in vec4 a_BoneWeights;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoords;

layout (set = 0, binding = 0) uniform UniformBufferObject {
    mat4 u_Model;
    mat4 u_View;
    mat4 u_Projection;
    mat4 u_FinalBoneTransforms[MAX_BONES];
};

void main() {
    vec4 totalPosition = vec4(a_Position, 1.0);
    /*
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if (a_BoneIDs[i] < 0) {
            continue;
        } else if (a_BoneIDs[i] >= MAX_BONES) {
            totalPosition = vec4(a_Position, 1.0);
            break;
        }

        vec4 localPosition = u_FinalBoneTransforms[a_BoneIDs[i]] * vec4(a_Position, 1.0);
        totalPosition += localPosition * a_BoneWeights[i];
        vec3 localNormal = mat3(u_FinalBoneTransforms[a_BoneIDs[i]]) * a_Normal;
    }
    */

	v_Position = vec3(u_Model * totalPosition);
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
	v_TexCoords = a_TexCoords;

	gl_Position = u_Projection * u_View * vec4(v_Position, 1.0);
}
