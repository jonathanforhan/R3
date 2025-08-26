#pragma once

#include <array>
#include <vector>
#include <vulkan/vulkan.h>
#include "api/Types.hpp"

namespace R3 {

static constexpr usize MAX_SHADER_LIGHTS = 128; /// Maximum scene lights allowed in shader
static constexpr usize MAX_SHADER_BONES  = 128; /// Maximum bones allowed for a mesh by the shader

struct ViewProjection {
    alignas(16) fmat4 view;
    alignas(16) fmat4 projection;
};

struct VertexUniformBufferObject {
    alignas(16) fmat4 model;
    alignas(16) fmat4 view;
    alignas(16) fmat4 projection;
    alignas(16) fmat4 finalBoneTransforms[MAX_SHADER_BONES];
};

struct FragmentPushConstant {
    alignas(8) fvec2 cursorPosition;
    alignas(4) uint32 uid;
    alignas(4) uint32 selected;
};

struct PointLightShaderObject {
    alignas(16) fvec3 position;
    alignas(16) fvec3 color;
    alignas(4) float intensity;
};

struct FragmentUniformBufferObject {
    alignas(16) fvec3 cameraPosition;
    alignas(4) uint32 pbrFlags;
    alignas(4) uint32 lightCount;
    PointLightShaderObject pointLights[MAX_SHADER_LIGHTS];
};

struct VertexBindingSpecification {
    uint32 binding;
    uint32 stride;
};

/// Vertex used by shaders
struct Vertex {
    fvec3 position;
    fvec3 normal;
    fvec3 tangent;
    fvec3 bitangent;
    fvec2 textureCoords;
    ivec4 boneIDs;
    fvec4 weights;

    static VkVertexInputBindingDescription getBindingDescription() noexcept {
        const VkVertexInputBindingDescription vertexInputBindingDescription = {
            .binding   = 0,
            .stride    = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };
        return vertexInputBindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() noexcept {
        const std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription = {
            {
                .location = 0,
                .binding  = 0,
                .format   = VK_FORMAT_R32G32B32_SFLOAT,
                .offset   = offsetof(Vertex, position),
            },
            {
                .location = 1,
                .binding  = 0,
                .format   = VK_FORMAT_R32G32B32_SFLOAT,
                .offset   = offsetof(Vertex, normal),
            },
            {
                .location = 2,
                .binding  = 0,
                .format   = VK_FORMAT_R32G32B32_SFLOAT,
                .offset   = offsetof(Vertex, tangent),
            },
            {
                .location = 3,
                .binding  = 0,
                .format   = VK_FORMAT_R32G32B32_SFLOAT,
                .offset   = offsetof(Vertex, bitangent),
            },
            {
                .location = 4,
                .binding  = 0,
                .format   = VK_FORMAT_R32G32_SFLOAT,
                .offset   = offsetof(Vertex, textureCoords),
            },
            {
                .location = 5,
                .binding  = 0,
                .format   = VK_FORMAT_R32G32B32_SINT,
                .offset   = offsetof(Vertex, boneIDs),
            },
            {
                .location = 6,
                .binding  = 0,
                .format   = VK_FORMAT_R32G32B32A32_SFLOAT,
                .offset   = offsetof(Vertex, weights),
            },
        };
        return vertexInputAttributeDescription;
    }
};

} // namespace R3
