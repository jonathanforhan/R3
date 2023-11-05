#version 460

in vec3 v_Color;
in vec2 v_TexCoords;

out vec4 f_Color;

uniform sampler2D u_Texture;

void main() {
    f_Color = texture(u_Texture, v_TexCoords) * vec4(v_Color, 1.0);
}
