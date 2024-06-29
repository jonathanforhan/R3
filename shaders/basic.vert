#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

layout(location = 0) out vec3 v_Color;

void main() {
    gl_Position = a_Position;
    v_Color = a_Color;
}