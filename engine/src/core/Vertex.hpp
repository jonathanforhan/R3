#pragma once 
#include "api/Types.hpp"

namespace R3 {

struct Vertex {
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 textureCoords;
};

} // namespace R3