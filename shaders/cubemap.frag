#version 460
#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) in vec3 v_TexCoords;

layout (location = 0) out vec4 f_Color;

layout (binding = 2) uniform samplerCube u_Samplers[];

layout (push_constant) uniform FragmentPushConstants {
    uint c_iCubemap;
};

void main() {
	f_Color = texture(u_Samplers[c_iCubemap], v_TexCoords);
}