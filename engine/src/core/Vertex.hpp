#pragma once 
#include "api/Types.hpp"

namespace R3 {

constexpr auto MAX_BONE_INFLUENCE = 4;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 textureCoords;
    int32 boneIDs[MAX_BONE_INFLUENCE];
    float weights[MAX_BONE_INFLUENCE];
};

} // namespace R3