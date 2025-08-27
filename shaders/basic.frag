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

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;
};

layout (binding = 6) uniform LightBuffer {
	vec3 u_ViewPosition;
	uint u_Flags;
	uint u_NumLights;
	PointLight u_Lights[MAX_LIGHTS];
};

void main() {
	vec3 albedo = texture(u_Albedo, v_TexCoords).rgb;
	f_Color = vec4(albedo, 1.0);
}