////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-VertexBuffer.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "../ShaderObjects.hxx"
#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <span>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

struct VertexBufferSpecification {
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

    /// @brief
    /// @param spec
    /// @return VertexBuffer | InitializationFailure | AllocationFailure
    static Result<VertexBuffer> create(const VertexBufferSpecification& spec);

    ~VertexBuffer();

private:
    VkDevice m_device    = VK_NULL_HANDLE;
    uint32 m_vertexCount = 0;
};

} // namespace R3::vulkan

#endif // R3_VULKAN
