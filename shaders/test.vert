#version 460

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitanget;
layout(location = 4) in vec2 a_TexCoords;

layout(location = 0) out vec3 v_Color;

void main() {
    vec3 pos = (a_Position + vec3(0, 0, 2.0)) * 0.2;
    gl_Position = vec4(pos, 1.0);
    v_Color = vec3(1.0, 1.0, 0.0);
}
