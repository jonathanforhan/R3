#version 460
#extension GL_EXT_nonuniform_qualifier : require

#define M_PI 3.14159265359
#define SAMPLER_MAX 1024

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_TexCoords;
layout (location = 3) in vec4 v_PositionLightSpace;

layout (location = 0) out vec4 f_Color;
layout (location = 1) out uint f_EntityID;

layout (binding = 2) uniform sampler2D u_Samplers[];

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

layout (binding = 3) readonly buffer LightBuffer {
    PointLight u_Lights[];
};

layout (binding = 5) uniform sampler2D u_ShadowMap;

layout (push_constant, std140) uniform FragmentPushConstants {
layout(offset = 64)
    vec3 c_ViewPosition;
    uint c_NumLights;
    /* indices for textures in the u_Samplers array */
    uint c_iAlbedo;
    uint c_iMetallicRoughness; /* metalness B channel, roughness G channel */
    uint c_iNormal;
    uint c_iAmbientOcclusion;
    uint c_iEmissive;
    uint c_EntityID;
};

vec3 calcTangentNormal(sampler2D normal, vec2 texCoords) {
    // normals are passed as 2 channel
    vec2 rg = texture(normal, texCoords).rg;
    vec2 xy = rg * 2.0 - 1.0;
    float z = sqrt(1.0 - dot(xy, xy));
    vec3 tangentNormal = vec3(xy, z);

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

float calcShadow(vec4 posLightSpace) {
    // perform perspective divide
    vec3 projCoords = posLightSpace.xyz / posLightSpace.w;

    float currentDepth = projCoords.z;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

	float closestDepth = texture(u_ShadowMap, projCoords.xy).r;

	vec3 normal = normalize(v_Normal);
	vec3 lightDir = normalize(u_Lights[0].position - v_Position);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow;

	shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

	// f_Color = vec4(closestDepth, currentDepth, 0.0, 1.0);
	// f_Color = vec4(vec3(closestDepth), 1.0);
	// f_Color = vec4(vec3(currentDepth), 1.0);
	// f_Color = vec4(vec3(shadow), 1.0);

    return shadow;
}

void main() {
    vec4 albedo = vec4(1.0, 0.0, 1.0, 1.0);
    if (c_iAlbedo < SAMPLER_MAX) {
        albedo = texture(u_Samplers[c_iAlbedo], v_TexCoords);
    }

    vec2 mr     = vec2(0.0);
    if (c_iMetallicRoughness < SAMPLER_MAX) {
        mr = texture(u_Samplers[c_iMetallicRoughness], v_TexCoords).rg;
    }

    vec3 N      = vec3(0.0);
    if (c_iNormal < SAMPLER_MAX) {
        N = calcTangentNormal(u_Samplers[c_iNormal], v_TexCoords);
    }

    vec3 ao = vec3(1.0);
    if (c_iAmbientOcclusion < SAMPLER_MAX) {
        ao *= vec3(texture(u_Samplers[c_iAmbientOcclusion], v_TexCoords).r);
    }

    vec4 emission = vec4(0.0);
    if (c_iEmissive < SAMPLER_MAX) {
        emission = texture(u_Samplers[c_iEmissive], v_TexCoords);
    }

    vec3 V = normalize(c_ViewPosition - v_Position);

    float roughness = mr.r;
    float metallic = mr.g;

    // calc reflectance at normal incidence; if dieletric use F0 of 0.04 else use albedo color as F0
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo.rgb, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < c_NumLights; i++) {
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
        Lo += (kD * albedo.rgb / M_PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.1) * albedo.rgb * ao;
    vec3 color = ambient + Lo;

    // emission
    color += emission.rgb;

    // HDR tonemapping
    // color = color / (color + vec3(1.0));

    // gamma correct
    // color = pow(color, vec3(1.0 / gamma)); // to sRGB

    float shadow = calcShadow(v_PositionLightSpace);
    color *= (1.0 - shadow * 0.75);
    f_Color = vec4(color, 1.0);

    f_EntityID = c_EntityID;
}
