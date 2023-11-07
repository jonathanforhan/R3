#version 460

out vec4 f_Color;

in vec2 v_TexCoords;

uniform sampler2D u_TextureDiffuse;

void main() {
	f_Color = texture(u_TextureDiffuse, v_TexCoords);
}
