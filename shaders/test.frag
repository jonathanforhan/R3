#version 460

layout(location = 0) out vec4 f_Color;

layout(location = 0) in vec3 v_Color;

void main() {
    f_Color = vec4(v_Color, 1.0);
}