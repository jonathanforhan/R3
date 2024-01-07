#version 460

#define DEPTH_ARRAY_SCALE 2048

#define M_PI 3.14159265359
#define MAX_LIGHTS 128

#define ALBEDO_FLAG_BIT				(1 << 0)
#define METALLIC_ROUGHNESS_FLAG_BIT	(1 << 1)
#define NORMAL_FLAG_BIT				(1 << 2)
#define AMBIENT_OCCULSION_FLAG_BIT	(1 << 3)
#define EMISSIVE_FLAG_BIT			(1 << 4)
#define HAS_BIT(X, BIT) ((X & BIT) != 0)

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_TexCoords;

layout (location = 0) out vec4 f_Color;

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;
};

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
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

vec3 calcTangentNormal() {
	vec3 tangentNormal = texture(u_Normal, v_TexCoords).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(v_Position);
	vec3 Q2 = dFdy(v_Position);
	vec2 st1 = dFdx(v_TexCoords);
	vec2 st2 = dFdy(v_TexCoords);

	vec3 N = normalize(v_Normal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

float distributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = M_PI * denom * denom;

	return nom / denom;
}

float geometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = geometrySchlickGGX(NdotV, roughness);
	float ggx1 = geometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
	// Mouse Picking
	uint zIndex = uint(gl_FragCoord.z * DEPTH_ARRAY_SCALE);
	if (length(c_MousePosition - gl_FragCoord.xy) < 1) {
		s_Data[zIndex] = c_Uid;
	}

	// Render
	vec3 albedo = texture(u_Albedo, v_TexCoords).rgb;
	vec4 mr = texture(u_MetallicRoughness, v_TexCoords);
	float metallic = mr.b;
	float roughness = mr.g;

	vec3 ambientOcclusion = vec3(0.1);
	if (HAS_BIT(u_Flags, AMBIENT_OCCULSION_FLAG_BIT)) {
		ambientOcclusion *= texture(u_AmbientOcclusion, v_TexCoords).rgb;
	}

	vec3 N = calcTangentNormal();
	vec3 V = normalize(u_ViewPosition - v_Position);

	// calc reflectance at normal incidence; if dieletric use F0 of 0.04 else use albedo color as F0
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);

	for (int i = 0; i < u_NumLights; i++) {
		// per light radiance
		vec3 L = normalize(u_Lights[i].position - v_Position);
		vec3 H = normalize(V + L);
		float dist = length(u_Lights[i].position - v_Position);
		float attenuation = 1.0 / (dist * dist);
		vec3 radiance = u_Lights[i].color * u_Lights[i].intensity * attenuation;

		// cook-terrance BRDF
		float NDF = distributionGGX(N, H, roughness);
		float G = geometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		// kS is equal to Fresnel
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);

		// add to outgoing radiance Lo
		Lo += (kD * albedo / M_PI + specular) * radiance * NdotL;
	}

	vec3 ambient = vec3(0.01) * albedo * ambientOcclusion;

	vec3 color = ambient + Lo;

	// emission
	if (HAS_BIT(u_Flags, EMISSIVE_FLAG_BIT)) {
		color += pow(texture(u_Emissive, v_TexCoords).rgb, vec3(2.2));
	}

	// HDR tonemapping
	color = color / (color + vec3(1.0));

	// gamma correction
	color = pow(color, vec3(1.0 / 2.2));

	f_Color = vec4(color, 1.0);

	if (c_Uid == c_Selected) {
		f_Color += vec4(0.16, 0.08, -0.1, 0.0);
	}
}