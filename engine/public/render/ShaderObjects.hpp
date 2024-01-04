#pragma once

#include <array>
#include "api/Types.hpp"

namespace R3 {

static constexpr uint32 MAX_LIGHTS = 128; ///< Maximum Scene lights
static constexpr uint32 MAX_BONES = 128;  ///< Maximum Bones allowed for a mesh

struct R3_API ViewProjection {
    alignas(16) mat4 view;
    alignas(16) mat4 projection;
};

struct R3_API VertexUniformBufferObject {
    alignas(16) mat4 model;
    alignas(16) mat4 view;
    alignas(16) mat4 projection;
    alignas(16) mat4 finalBoneTransforms[MAX_BONES];
};

struct R3_API FragmentPushConstant {
    alignas(8) vec2 cursorPosition;
    alignas(4) uint32 uid;
    alignas(4) uint32 selected;
};

struct R3_API PointLightShaderObject {
    alignas(16) vec3 position;
    alignas(16) vec3 color;
    alignas(4) float intensity;
};

struct R3_API FragmentUniformBufferObject {
    alignas(16) vec3 cameraPosition;
    alignas(4) uint32 pbrFlags;
    alignas(4) uint32 lightCount;
    PointLightShaderObject pointLights[MAX_LIGHTS];
};

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
    vec3 position;      ///< vertex position data
    vec3 normal;        ///< vertex normal data
    vec3 tangent;       ///< vertex tangent data
    vec3 bitangent;     ///< vertex bitangent data
    vec2 textureCoords; ///< vertex texture coordinate data
    ivec4 boneIDs;      ///< identifier of bone IDs from animations
    vec4 weights;       ///< bone weights used from animations

    [[nodiscard]] static constexpr VertexBindingSpecification vertexBindingSpecification() {
        return {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VertexInputRate::Vertex,
        };
    }

    [[nodiscard]] static constexpr std::array<VertexAttributeSpecification, 7> vertexAttributeSpecification() {
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
            },
            {
                .location = 5,
                .binding = 0,
                .format = Format::R32G32B32Sint,
                .offset = offsetof(Vertex, boneIDs),
            },
            {
                .location = 6,
                .binding = 0,
                .format = Format::R32G32B32A32Sfloat,
                .offset = offsetof(Vertex, weights),
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
