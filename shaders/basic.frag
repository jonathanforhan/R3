#version 460
#extension GL_EXT_nonuniform_qualifier : require

#define M_PI 3.14159265359

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_TexCoords;

layout (location = 0) out vec4 f_Color;

layout (binding = 2) uniform sampler2D u_Samplers[];

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

layout (binding = 3) uniform LightBuffer {
    PointLight u_Lights[];
};

layout (push_constant) uniform FragmentPushConstants {
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
    // Render
    vec3 albedo = texture(u_Samplers[c_iAlbedo], v_TexCoords).rgb;

    vec3 color = albedo;

    // emission
    if (c_iEmissive != 0xffffffff) {
        color += texture(u_Samplers[c_iEmissive], v_TexCoords).rgb;
    }

    f_Color = vec4(color, 1.0);
}
