#version 450

layout(location = 0) in vec3 v_Color;

layout(location = 0) out vec4 f_Color;

void main() {
    f_Color = vec4(fragColor, 1.0);
}