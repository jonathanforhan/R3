#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"

namespace R3 {

class RenderContext;
struct Queue;

enum class CommandPoolMode {
    Protected, // VK_COMMAND_POOL_CREATE_PROTECTED_BIT - for protected memory
    Reset,     // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT - individual command buffer reset
    Transient  // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT - short-lived command buffers
};

class CommandAllocator {
public:
    void create(RenderContext& ctx, uint32 queueIndex, CommandPoolMode mode) noexcept(false);

    void destroy() noexcept(true);

    VkCommandBuffer allocateBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) noexcept(false);

    std::vector<VkCommandBuffer> allocateBuffers(
        uint32_t count,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) noexcept(false);

    void freeBuffer(VkCommandBuffer commandBuffer) noexcept(true);

    void freeBuffers(const std::vector<VkCommandBuffer>& commandBuffers) noexcept(true);

    void reset(VkCommandPoolResetFlags flags = 0) noexcept(false);

    void executeImmediate(Queue& queue, std::function<void(VkCommandBuffer)> function) noexcept(false);

    VkCommandPool handle() const { return m_pool; }
    CommandPoolMode mode() const { return m_mode; }

    bool isValid() const { return m_pool != VK_NULL_HANDLE; }

private:
    VkCommandPoolCreateFlags getModeFlags(CommandPoolMode mode) const noexcept(true);

private:
    VkDevice m_device      = VK_NULL_HANDLE;
    VkCommandPool m_pool   = VK_NULL_HANDLE;
    CommandPoolMode m_mode = CommandPoolMode::Reset;
};

} // namespace R3