#version 460

in vec3 v_TexCoords;

out vec4 f_Color;

uniform samplerCube u_Skybox;

void main() {
	f_Color = texture(u_Skybox, v_TexCoords);
}
