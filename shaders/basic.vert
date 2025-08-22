#version 460

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

layout(location = 0) out vec3 v_Color;
layout(location = 1) out vec2 v_TexCoord;

layout(binding = 0) uniform MVP {
    mat4 model;
    mat4 view;
    mat4 proj;
} u_MVP;

void main() {
    gl_Position = u_MVP.proj * u_MVP.view * u_MVP.model * vec4(a_Position, 1.0);
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
}