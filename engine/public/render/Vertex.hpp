#pragma once

#include <array>
#include "api/Types.hpp"
#include "render/RenderApi.hpp"
#include "render/ShaderObjects.hpp"

namespace R3 {

constexpr auto MAX_BONE_INFLUENCE = 4; ///< @brief Maximum weights that can be attached to a bone, mirrored in shader

struct R3_API VertexBindingSpecification {
    uint32 binding;
    uint32 stride;
    VertexInputRate inputRate;
};

struct R3_API VertexAttributeSpecification {
    uint32 location;
    uint32 binding;
    Format format;
    uint32 offset;
};

/// @brief Vertex used for Rendering, currently using an array of structure rendering model
struct R3_API Vertex {
    vec3 position;                     ///< vertex position data
    vec3 normal;                       ///< vertex normal data
    vec3 tangent;                      ///< vertex tangent data
    vec3 bitangent;                    ///< vertex bitangent data
    vec2 textureCoords;                ///< vertex texture coordinate data
    int32 boneIDs[MAX_BONE_INFLUENCE]; ///< identifier of bone IDs from animations
    float weights[MAX_BONE_INFLUENCE]; ///< bone weights used from animations

    [[nodiscard]] static constexpr VertexBindingSpecification vertexBindingSpecification() {
        return {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VertexInputRate::Vertex,
        };
    }

    [[nodiscard]] static constexpr std::array<VertexAttributeSpecification, 5> vertexAttributeSpecification() {
        return {
            VertexAttributeSpecification{
                .location = 0,
                .binding = 0,
                .format = Format::R32G32B32Sfloat,
                .offset = offsetof(Vertex, position),
            },
            VertexAttributeSpecification{
                .location = 1,
                .binding = 0,
                .format = Format::R32G32B32Sfloat,
                .offset = offsetof(Vertex, normal),
            },
            VertexAttributeSpecification{
                .location = 2,
                .binding = 0,
                .format = Format::R32G32B32Sfloat,
                .offset = offsetof(Vertex, tangent),
            },
            VertexAttributeSpecification{
                .location = 3,
                .binding = 0,
                .format = Format::R32G32B32Sfloat,
                .offset = offsetof(Vertex, tangent),
            },
            VertexAttributeSpecification{
                .location = 4,
                .binding = 0,
                .format = Format::R32G32Sfloat,
                .offset = offsetof(Vertex, textureCoords),
#if 0 // TODO ANIMATION
            },
            {
                .location = 5,
                .binding = 0,
                .format = Format::R32Sint,
                .offset = offsetof(Vertex, boneIDs),
            },
            {
                .location = 6,
                .binding = 0,
                .format = Format::R32Sfloat,
                .offset = offsetof(Vertex, weights),
#endif
            },
        };
    }
};

#if not R3_BUILD_DIST
/// @brief Vertex used for Object Picking
struct R3_API ObjectPickerVertex {
    vec3 position; ///< vertex position data
    uint32 id;     ///< Entity id

    [[nodiscard]] static constexpr VertexBindingSpecification vertexBindingSpecification() {
        return {
            .binding = 0,
            .stride = sizeof(ObjectPickerVertex),
            .inputRate = VertexInputRate::Vertex,
        };
    }

    [[nodiscard]] static constexpr std::array<VertexAttributeSpecification, 2> vertexAttributeSpecification() {
        return {
            VertexAttributeSpecification{
                .location = 0,
                .binding = 0,
                .format = Format::R32G32B32Sfloat,
                .offset = offsetof(ObjectPickerVertex, position),
            },
            VertexAttributeSpecification{
                .location = 1,
                .binding = 0,
                .format = Format::R32Uint,
                .offset = offsetof(ObjectPickerVertex, id),
            },
        };
    }
};
#endif // not R3_BUILD_DIST

} // namespace R3