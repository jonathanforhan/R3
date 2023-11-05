#version 460

out vec4 f_Color;

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform vec3 u_ViewPosition;
uniform vec3 u_Color;
uniform Light u_Light;

void main() {
	// Ambient
	vec3 ambient = u_Light.ambient * u_Color;

	// Diffuse
	vec3 norm = normalize(v_Normal);
	vec3 lightDirection = normalize(u_Light.position - v_Position);
	float difference = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = u_Light.diffuse * difference * u_Color;

	// Specular
	vec3 viewDirection = normalize(u_ViewPosition - v_Position);
	vec3 reflectionDirection = reflect(-lightDirection, norm);
	float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), 32);
	vec3 specular = u_Light.specular * spec * u_Color;

	vec3 result = ambient + diffuse + specular;
	f_Color = vec4(result, 1.0);
}
