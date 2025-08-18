#pragma once

#include <functional>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

class CommandAllocator {
public:
    void create(RenderContext& ctx, uint32 queueIndex, VkCommandPoolCreateFlags mode);

    void destroy() noexcept;

    VkCommandBuffer allocateBuffer(bool primary = true);

    std::vector<VkCommandBuffer> allocateBuffers(uint32 count, bool primary = true);

    void freeBuffer(VkCommandBuffer commandBuffer) noexcept;

    void freeBuffers(std::span<VkCommandBuffer> commandBuffers) noexcept;

    void reset(bool releaseResources = false);

    void executeImmediate(VkQueue queue, std::function<void(VkCommandBuffer)> function);

    VkCommandPoolCreateFlags mode() const noexcept { return m_mode; }

private:
    VkDevice m_device               = VK_NULL_HANDLE;
    VkCommandPool m_pool            = VK_NULL_HANDLE;
    VkCommandPoolCreateFlags m_mode = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
};

} // namespace R3::vulkan