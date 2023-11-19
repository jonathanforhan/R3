#version 460

#define MAX_LIGHTS 10

// flag bits
// bit is set if that shader is present
#define BASE_COLOR_FLAG	(1 << 0)
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

void main() {
	f_Color = vec4(vec3(texture(u_Material.diffuse, v_TexCoords)), 1.0);
}
