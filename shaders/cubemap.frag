#version 460

layout(location = 0) in vec3 v_TexCoords;

layout(location = 0) out vec4 f_Color;

layout(binding = 0) uniform samplerCube u_Skybox;

void main() {
	f_Color = texture(u_Skybox, v_TexCoords);
}
