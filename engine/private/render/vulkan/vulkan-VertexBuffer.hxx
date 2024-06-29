#pragma once

#if R3_VULKAN

#include "../ShaderObjects.hxx"
#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Types.hpp>
#include <span>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

struct VertexBufferSpecfication {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& device;
    const CommandBuffer& commandBuffer;
    std::span<const Vertex> vertices;
};

class VertexBuffer : public VulkanObject<VkBuffer> {
public:
    DEFAULT_CONSTRUCT(VertexBuffer);
    NO_COPY(VertexBuffer);
    DEFAULT_MOVE(VertexBuffer);

    // VertexBuffer();
};

} // namespace R3::vulkan

#endif // R3_VULKAN
