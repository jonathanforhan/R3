#version 460

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Bitanget;
layout (location = 4) in vec2 a_TexCoords;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoords;

layout (set = 0, binding = 0) uniform UniformBufferObject {
    mat4 u_Model;
    mat4 u_View;
    mat4 u_Projection;
};

void main() {
    vec3 pos = (a_Position + vec3(0.0, 0.0, 1.0)) * 0.5;
	v_Position = vec3(u_Model * vec4(pos, 1.0));

    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;

	v_TexCoords = a_TexCoords;

	gl_Position = u_Projection * u_View * vec4(v_Position, 1.0);
}
