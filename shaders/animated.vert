#version 460

#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4

out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_TexCoords;
out flat highp uint v_Flags;

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Bitanget;
layout (location = 4) in vec2 a_TexCoords;
layout (location = 5) in ivec4 a_BoneIDs;
layout (location = 6) in vec4 a_Weights;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec2 u_Tiling;
uniform highp uint u_Flags;
uniform mat4 u_BoneTransforms[MAX_BONES];

void main() {
    vec4 position = vec4(0.0);
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if (a_BoneIDs[i] == -1) {
            continue;
        }
        if (a_BoneIDs[i] >= MAX_BONES) {
            position = vec4(a_Position, 1.0);
            break;
        }
        vec4 localPosition = u_BoneTransforms[a_BoneIDs[i]] * vec4(a_Position, 1.0);
        position += localPosition * a_Weights[i];
        vec3 localNormal = mat3(u_BoneTransforms[a_BoneIDs[i]]) * a_Normal;
    }

    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);

    v_Position = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    v_TexCoords = a_TexCoords * u_Tiling;
    v_Flags = u_Flags;
}

