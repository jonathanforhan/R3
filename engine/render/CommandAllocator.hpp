#pragma once

#include <cstdint>
#include <functional>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Flags.hpp"
#include "Types.hpp"

namespace R3 {

class RenderContext;
struct Queue;

class CommandAllocator {
public:
    void create(RenderContext& ctx, uint32 queueIndex, CommandPoolMode mode);

    void destroy() noexcept;

    VkCommandBuffer allocateBuffer(bool primary = true);

    std::vector<VkCommandBuffer> allocateBuffers(uint32_t count, bool primary = true);

    void freeBuffer(VkCommandBuffer commandBuffer) noexcept;

    void freeBuffers(std::span<VkCommandBuffer> commandBuffers) noexcept;

    void reset(bool releaseResources = false);

    void executeImmediate(Queue& queue, std::function<void(VkCommandBuffer)> function);

    CommandPoolMode mode() const noexcept { return m_mode; }

    bool isValid() const noexcept { return m_pool != VK_NULL_HANDLE; }

private:
    VkDevice m_device      = VK_NULL_HANDLE;
    VkCommandPool m_pool   = VK_NULL_HANDLE;
    CommandPoolMode m_mode = CommandPoolModeFlags::Reset;
};

} // namespace R3