#version 460

uniform layout (binding = 1) sampler2D u_Ambient;
uniform	layout (binding = 2) sampler2D u_Normal;
uniform	layout (binding = 3) sampler2D u_MetallicRoughness; // metalness B channel, roughness G channel
uniform	layout (binding = 4) sampler2D u_AmbientOcclusion;
uniform	layout (binding = 5) sampler2D u_Emissive;

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_TexCoords;

layout (location = 0) out vec4 f_Color;


void main() {
    f_Color = texture(u_Ambient, v_TexCoords);
}