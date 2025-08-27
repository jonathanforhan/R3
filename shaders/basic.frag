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
    vec3 u_ViewPosition;
    uint u_NumLights;
    PointLight u_Lights[];
};

/* contains indices for textures in the u_Samplers array */
layout (push_constant) uniform Material {
    uint iAlbedo;
    uint iMetallicRoughness; /* metalness B channel, roughness G channel */
    uint iNormal;
    uint iAmbientOcclusion;
    uint iEmissive;
} material;

void main() {
    vec3 albedo = texture(u_Samplers[nonuniformEXT(material.iAlbedo)], v_TexCoords).rgb;
    f_Color = vec4(albedo, 1.0);
}