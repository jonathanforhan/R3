#version 460
#extension GL_GOOGLE_include_directive : require

#define DEPTH_ARRAY_SCALE 2048

#define M_PI 3.14159265359
#define MAX_LIGHTS 128

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_TexCoords;

layout (location = 0) out vec4 f_Color;

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;
};

// textures
layout (binding = 1) uniform sampler2D u_Albedo;
layout (binding = 2) uniform sampler2D u_MetallicRoughness; // metalness B channel, roughness G channel
layout (binding = 3) uniform sampler2D u_Normal;
layout (binding = 4) uniform sampler2D u_AmbientOcclusion;
layout (binding = 5) uniform sampler2D u_Emissive;

// uniform
layout (binding = 6) uniform LightBuffer {
	vec3 u_ViewPosition;
    uint u_Flags;
	uint u_NumLights;
	PointLight u_Lights[MAX_LIGHTS];
};

// ssbo
layout (set = 0, binding = 7) buffer SSBO {
    uint s_Data[DEPTH_ARRAY_SCALE];
};

// push constant
layout (push_constant) uniform FragmentPushConstant {
    vec2 c_MousePosition;
    uint c_Uid;
    uint c_Selected;
};

void main() {
	// Mouse Picking
	uint zIndex = uint(gl_FragCoord.z * DEPTH_ARRAY_SCALE);
	if (length(c_MousePosition - gl_FragCoord.xy) < 1) {
		s_Data[zIndex] = c_Uid;
	}

	// Render
	vec3 albedo = texture(u_Albedo, v_TexCoords).rgb;

	f_Color = vec4(albedo, 1.0);

	if (c_Uid == c_Selected) {
		f_Color += vec4(0.16, 0.08, -0.1, 0.0);
	}
}
