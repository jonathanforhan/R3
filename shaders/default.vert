#version 460

layout (location = 0) in vec3 aPosition;

out vec4 vColor;

void main() {
    gl_Position = vec4(aPosition, 1.0);
    vColor = vec4(0, 0, 1, 1);
}

