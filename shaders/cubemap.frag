#version 460
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec3 v_TexCoords;

layout(location = 0) out vec4 f_Color;

layout (binding = 2) uniform samplerCube u_Samplers[];

layout (push_constant) uniform FragmentPushConstants {
layout(offset = 64)
    vec3 c_ViewPosition;
    uint c_NumLights;
    /* indices for textures in the u_Samplers array */
    uint c_iAlbedo;
    uint c_iMetallicRoughness; /* metalness B channel, roughness G channel */
    uint c_iNormal;
    uint c_iAmbientOcclusion;
    uint c_iEmissive;
};


void main() {
	f_Color = texture(u_Samplers[c_iAlbedo], v_TexCoords);
}