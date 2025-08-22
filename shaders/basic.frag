#version 460

layout(location = 0) in vec3 v_Color;
layout(location = 1) in vec2 v_TexCoord;

layout(location = 0) out vec4 f_Color;

layout(binding = 1) uniform sampler2D u_Texture;

void main() {
    f_Color = texture(u_Texture, v_TexCoord);
}