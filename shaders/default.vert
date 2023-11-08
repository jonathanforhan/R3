#version 460

out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_TexCoords;
out flat highp uint v_Flags;

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec3 a_Bitanget;
layout (location = 4) in vec2 a_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec2 u_Tiling;
uniform highp uint u_Flags;

void main() {
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);

    v_Position = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    v_TexCoords = a_TexCoords * u_Tiling;
    v_Flags = u_Flags;
}

