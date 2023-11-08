#version 460

#define NUM_LIGHTS 1

out vec4 f_Color;

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoords;

uniform sampler2D u_TextureDiffuse;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
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

	vec3 color;
};

uniform vec3 u_ViewPosition;
uniform Material u_Material;
uniform Light u_Lights[NUM_LIGHTS];

vec3 calcLight(Light light, vec3 normal, vec3 position, vec3 viewDirection);

void main() {
	vec3 norm = normalize(v_Normal);
	vec3 viewDirection = normalize(u_ViewPosition - v_Position);

	vec3 result = vec3(0.0);

	for (int i = 0; i < NUM_LIGHTS; i++) {
		result += calcLight(u_Lights[i], norm, v_Position, viewDirection);
	}

	f_Color = vec4(result, 1.0);
}

vec3 calcLight(Light light, vec3 normal, vec3 position, vec3 viewDirection) {
	vec3 lightDirection = normalize(light.position - position);

	// diffuse
	float diff = max(dot(normal, lightDirection), 0.0);

	// specular
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), u_Material.shininess);

	// attenuation
	float distance = length(light.position - position);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// result
	vec3 ambient = light.ambient * vec3(texture(u_Material.diffuse, v_TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(u_Material.diffuse, v_TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(u_Material.specular, v_TexCoords));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular) * light.color;
}
