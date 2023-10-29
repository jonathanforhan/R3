#version 460

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_coord;

out vec3 v_color;
out vec2 v_tex_coord;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0);
    v_color = color;
    v_tex_coord = tex_coord;
}

