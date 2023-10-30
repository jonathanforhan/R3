#version 460

out vec4 f_color;

in vec3 v_color;
in vec2 v_tex_coord;

uniform sampler2D u_texture;

void main() {
    f_color = texture(u_texture, v_tex_coord); // * vec4(v_color, 1.0);
}
