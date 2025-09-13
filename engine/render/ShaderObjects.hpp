#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "engine/api/Types.hpp"

namespace R3 {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PBR
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct R3_API PBRVertexUniformBufferObject {
    alignas(16) fmat4 view;
    alignas(16) fmat4 projection;
    alignas(16) fmat4 lightViewProjection;
};

struct R3_API PBRVertexPushConstants {
    alignas(16) fmat4 model;
};

struct R3_API PBRFragmentPushConstants {
    alignas(16) fvec3 viewPosition;
    alignas(4) uint32 numLights;
    alignas(4) uint32 iAlbedo;
    alignas(4) uint32 iMetallicRoughness;
    alignas(4) uint32 iNormal;
    alignas(4) uint32 iAmbientOcclusion;
    alignas(4) uint32 iEmissive;
    alignas(4) uint32 bSelected;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Editor
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct R3_API EditorVertexUniformBufferObject {
    alignas(16) fmat4 view;
    alignas(16) fmat4 projection;
};

struct R3_API EditorVertexPushConstants {
    alignas(16) fmat4 model;
};

struct R3_API EditorFragmentPushConstants {
    alignas(4) uint32 entityID;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Cubemap
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct R3_API CubemapFragmentPushConstants {
    alignas(4) uint32 iCubemap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Shadow
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct R3_API ShadowVertexPushConstants {
    alignas(16) fmat4 model;
    alignas(16) fmat4 lightViewProj;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Generic
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct R3_API PointLightShaderObject {
    alignas(16) fvec3 position;
    alignas(16) fvec3 color;
    alignas(4) float intensity;
};

/// Vertex used by shaders
struct R3_API Vertex {
    fvec3 position;
    fvec3 normal;
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
                .format   = VK_FORMAT_R32G32_SFLOAT,
                .offset   = offsetof(Vertex, textureCoords),
            },
            {
                .location = 3,
                .binding  = 0,
                .format   = VK_FORMAT_R32G32B32_SINT,
                .offset   = offsetof(Vertex, boneIDs),
            },
            {
                .location = 4,
                .binding  = 0,
                .format   = VK_FORMAT_R32G32B32A32_SFLOAT,
                .offset   = offsetof(Vertex, weights),
            },
        };
        return vertexInputAttributeDescription;
    }
};

} // namespace R3
