#version 460

layout (location = 0) in vec3 a_Position;

out vec3 v_TexCoords;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
	v_TexCoords = a_Position;
	vec4 position = u_Projection * u_View * vec4(a_Position, 1.0);
	gl_Position = position.xyww;
}
