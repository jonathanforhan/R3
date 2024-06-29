#pragma once

#if R3_VULKAN

#include "vulkan-CommandBuffer.hxx"
#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Types.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

struct CommandPoolSpecification {
    const LogicalDevice& device;
    const Queue& queue;
    VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    uint32 commandBufferCount;
};

class CommandPool : public VulkanObject<VkCommandPool> {
public:
    DEFAULT_CONSTRUCT(CommandPool);
    NO_COPY(CommandPool);
    DEFAULT_MOVE(CommandPool);

    CommandPool(const CommandPoolSpecification& spec);

    ~CommandPool();

    CommandBuffer operator[](int i) { return m_commandBuffers[i]; }

private:
    VkDevice m_device = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;
};

} // namespace R3::vulkan

#endif // R3_VULKAN