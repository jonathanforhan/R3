#version 450

layout(location = 0) in vec3 a_Position;

layout(push_constant) uniform VertexPushConstants {
    mat4 c_Model;
} pc;

void main() {
    // Just transform to world space, geometry shader handles projection
    gl_Position = pc.c_Model * vec4(a_Position, 1.0);
}