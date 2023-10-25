#version 460

out vec4 f_color;

in vec3 v_color;
in vec2 v_tex_coord;

uniform sampler2D uni_texture;

void main() {
    f_color = texture(uni_texture, v_tex_coord) * vec4(v_color, 1.0);
}
