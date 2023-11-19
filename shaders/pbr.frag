#version 460

#define M_PI 3.14159265359
#define MAX_LIGHTS 10

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoords;
in flat highp uint v_Flags;

out vec4 f_Color;

struct Light {
	vec3 position;
	vec3 color;
};

uniform	layout (binding = 0) sampler2D u_Albedo;
uniform	layout (binding = 1) sampler2D u_Normal;
uniform	layout (binding = 2) sampler2D u_MetallicRoughness; // metalness B channel, roughness G channel
uniform	layout (binding = 3) sampler2D u_AmbientOcclusion;
uniform	layout (binding = 4) sampler2D u_Emissive;

uniform vec3 u_ViewPosition;
uniform Light u_Lights[MAX_LIGHTS];
uniform lowp uint u_NumLights;

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
	vec3 albedo = pow(texture(u_Albedo, v_TexCoords).rgb, vec3(2.2));
	vec4 mr = texture(u_MetallicRoughness, v_TexCoords);
	float metallic = mr.b;
	float roughness = mr.g;
	vec3 ambientOcclusion = texture(u_AmbientOcclusion, v_TexCoords).rgb;

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
		vec3 radiance = u_Lights[i].color * attenuation;

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
	color += pow(texture(u_Emissive, v_TexCoords).rgb, vec3(2.2));

	// HDR tonemapping
	color = color / (color + vec3(1.0));

	// gamma correction
	color = pow(color, vec3(1.0 / 2.2));

	f_Color = vec4(color, 1.0);
}