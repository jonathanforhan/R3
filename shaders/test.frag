#version 460

layout (binding = 1) uniform sampler2D u_TexSampler;

layout (location = 0) in vec3 v_Color;
layout (location = 1) in vec2 v_TexCoords;

layout (location = 0) out vec4 f_Color;

void main() {
    f_Color = texture(u_TexSampler, v_TexCoords);
    
}