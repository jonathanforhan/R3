#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out; // 6 faces * 3 vertices

layout(location = 0) out vec4 g_WorldPos;

layout(push_constant) uniform GeometryPushConstants {
layout(offset = 64)
    vec3 c_LightPos;
};

layout(binding = 4) uniform ShadowViews {
    mat4 u_ShadowViews[6];
};

void main() {
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face; // Render to cubemap face
        
        for(int i = 0; i < 3; ++i) {
            vec4 worldPos = gl_in[i].gl_Position;
            g_WorldPos = worldPos;
            gl_Position = u_ShadowViews[face] * worldPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}