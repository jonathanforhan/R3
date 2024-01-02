#version 460

layout (set = 0, binding = 0) uniform UniformBufferObject {
    mat4 u_Model;
};

layout (push_constant) uniform ViewProjection {
    mat4 c_View;
    mat4 c_Projection;
};

layout (location = 0) in vec3 a_Position;
layout (location = 1) in uint a_ID;

layout (location = 0) out uint v_ID;

void main() {
	gl_Position = c_Projection * c_View * u_Model * vec4(a_Position, 1.0);
    v_ID = a_ID;
}
