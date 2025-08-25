#version 460

#define M_PI 3.14159265359
#define MAX_LIGHTS 128

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_TexCoords;

layout (location = 0) out vec4 f_Color;

layout (binding = 1) uniform sampler2D u_Albedo;
layout (binding = 2) uniform sampler2D u_MetallicRoughness; // metalness B channel, roughness G channel
layout (binding = 3) uniform sampler2D u_Normal;
layout (binding = 4) uniform sampler2D u_AmbientOcclusion;
layout (binding = 5) uniform sampler2D u_Emissive;

void main() {
	// f_Color = texture(u_Albedo, v_TexCoords);
	f_Color = vec4(v_Normal * 0.5 + 0.5, 1.0);
}