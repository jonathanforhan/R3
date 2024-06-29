#pragma once

#if R3_VULKAN
#include <vulkan/vulkan.h>
#endif

#include <api/Types.hpp>
#include <array>

namespace R3 {

struct Vertex {
    vec3 position;
    vec3 color;

#if R3_VULKAN
    static consteval VkVertexInputBindingDescription vertexInputBindingDescription() {
        return VkVertexInputBindingDescription{
            .binding   = 0,
            .stride    = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };
    }

    static consteval std::array<VkVertexInputAttributeDescription, 2> vertexInputAttributeDescriptions() {
        return {
            VkVertexInputAttributeDescription{
                .location = 0,
                .binding  = 0,
                .format   = VK_FORMAT_R32G32B32_SFLOAT,
                .offset   = offsetof(Vertex, position),
            },
            VkVertexInputAttributeDescription{
                .location = 0,
                .binding  = 1,
                .format   = VK_FORMAT_R32G32B32_SFLOAT,
                .offset   = offsetof(Vertex, color),
            },
        };
    }
#endif
};

} // namespace R3