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

layout (set = 0, binding = 0) uniform UniformBufferObject {
    mat4 u_Model;
    mat4 u_View;
    mat4 u_Projection;
    mat4 u_FinalJointTransforms[MAX_JOINTS];
};

void main() {
    vec4 animatedPosition = vec4(0.0f);
    mat4 jointTransform = mat4(0.0f);

    for (int i = 0; i < MAX_JOINT_INFLUENCE; i++) {
        if (a_JointIDs[i] < 0) {
            continue;
        }

        if (a_JointIDs[i] >= MAX_JOINTS) {
            animatedPosition = vec4(a_Position, 1.0f);
            jointTransform = mat4(1.0f);
            break;
        }

        vec4 localPosition = u_FinalJointTransforms[a_JointIDs[i]] * vec4(a_Position, 1.0f);
        animatedPosition += localPosition * a_Weights[i];
        jointTransform += u_FinalJointTransforms[a_JointIDs[i]] * a_Weights[i];
    }

	v_Position = vec3(u_Model * animatedPosition);
    v_Normal = mat3(transpose(inverse(u_Model * jointTransform))) * a_Normal;
    // v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
	v_TexCoords = a_TexCoords;

    gl_Position = u_Projection * u_View * vec4(v_Position, 1.0);
}
