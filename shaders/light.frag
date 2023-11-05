#version 460

out vec4 f_Color;

uniform vec3 u_Color;

void main() {
	f_Color = vec4(u_Color, 1.0);
}