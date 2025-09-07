#version 460

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in ivec4 a_JointIDs;
layout (location = 4) in vec4 a_Weights;

layout (push_constant) uniform DirectionalShadowPushConstants {
	mat4 u_Model;
	mat4 u_LightViewProj;
};

void main() {
	gl_Position = u_LightViewProj * u_Model * vec4(a_Position, 1.0);
}