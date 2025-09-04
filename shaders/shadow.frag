#version 450

layout(push_constant) uniform GeometryPushConstants {
layout(offset = 64)
    vec3 c_LightPos;
};

layout(location = 0) in vec4 g_WorldPos; // You'll need this from geometry shader

void main() {
    // Calculate distance from light to fragment
    float distance = length(g_WorldPos.xyz - c_LightPos);
    
    // Normalize to [0,1] range using far plane
    gl_FragDepth = distance / 25.0; // 25.0 is your far plane
}