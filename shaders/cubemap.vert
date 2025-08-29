#version 460

layout(location = 0) in vec3 a_Position;

layout(location = 0) out vec3 v_TexCoords;

layout(binding = 0) uniform ModelViewProjection {
    mat4 u_View;
    mat4 u_Proj;
};

void main() {
	v_TexCoords = a_Position;
	vec4 position = u_Proj * mat4(mat3(u_View)) * vec4(a_Position, 1.0);
	gl_Position = position.xyww;
}

