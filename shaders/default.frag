#version 460

#define MAX_LIGHTS 20

// flag bits
// bit is set if that shader is present
#define DIFFUSE_FLAG	(1 << 0)
#define SPECULAR_FLAG   (1 << 1)
#define NORMAL_FLAG		(1 << 2)
#define HEIGHT_FLAG		(1 << 3)
#define EMISSIVE_FLAG	(1 << 4)

out vec4 f_Color;

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoords;
flat in highp uint v_Flags;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D height;
	sampler2D emissive;
	float shininess;
};

struct Light {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emissive;

	vec3 color;
};

uniform vec3 u_ViewPosition;
uniform Material u_Material;
uniform Light u_Lights[MAX_LIGHTS];
uniform lowp uint u_NumLights;

vec3 calcLight(uint index, vec3 normal, vec3 position, vec3 viewDirection, highp uint flags);

void main() {
	vec3 norm = normalize(v_Normal);
	vec3 viewDirection = normalize(u_ViewPosition - v_Position);

	vec3 result = vec3(0.0);

	for (int i = 0; i < u_NumLights; i++) {
		result += calcLight(i, norm, v_Position, viewDirection, v_Flags);
	}

	f_Color = vec4(result, 1.0);
}

vec3 calcLight(uint index, vec3 normal, vec3 position, vec3 viewDirection, highp uint flags) {
	vec3 lightDirection = normalize(u_Lights[index].position - position);

	// diffuse
	float diff = max(dot(normal, lightDirection), 0.0);

	// specular
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), u_Material.shininess);

	// attenuation
	float distance = length(u_Lights[index].position - position);
	float attenuation = 1.0 / (u_Lights[index].constant + u_Lights[index].linear * distance + u_Lights[index].quadratic * (distance * distance));

	// result
	vec3 ambient = u_Lights[index].ambient * vec3(texture(u_Material.diffuse, v_TexCoords));
	vec3 diffuse = u_Lights[index].diffuse * diff * vec3(texture(u_Material.diffuse, v_TexCoords));
	vec3 specular = u_Lights[index].specular * spec * vec3(texture(u_Material.specular, v_TexCoords));
	vec3 emissive = u_Lights[index].emissive * vec3(texture(u_Material.emissive, v_TexCoords));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	emissive *= attenuation;

	diffuse *= int(bool(flags & DIFFUSE_FLAG));
	specular *= int(bool(flags & SPECULAR_FLAG));
	emissive *= int(bool(flags & EMISSIVE_FLAG));

	return (ambient + diffuse + specular + emissive) * u_Lights[index].color;
}
