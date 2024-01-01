#pragma once

#include "api/Types.hpp"

namespace R3 {

constexpr auto MAX_BONE_INFLUENCE = 4; ///< @brief Maximum weights that can be attached to a bone, mirrored in shader

/// @brief Vertex used for Rendering, currently using an array of structure rendering model
struct Vertex {
    vec3 position;                     ///< vertex position data
    vec3 normal;                       ///< vertex normal data
    vec3 tangent;                      ///< vertex tangent data
    vec3 bitangent;                    ///< vertex bitangent data
    vec2 textureCoords;                ///< vertex texture coordinate data
    int32 boneIDs[MAX_BONE_INFLUENCE]; ///< identifier of bone IDs from animations
    float weights[MAX_BONE_INFLUENCE]; ///< bone weights used from animations
};

} // namespace R3