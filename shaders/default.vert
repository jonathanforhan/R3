#version 460

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;
layout (location = 2) in vec2 a_TexCoords;

out vec3 v_Color;
out vec2 v_TexCoords;

layout (location = 0) uniform mat4 u_Model;
layout (location = 1) uniform mat4 u_View;
layout (location = 2) uniform mat4 u_Projection;

void main() {
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    v_Color = a_Color;
    v_TexCoords = a_TexCoords;
}

