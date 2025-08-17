#if R3_VULKAN

#include "render/CommandAllocator.hpp"

#include <cstdint>
#include <functional>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Exception.hpp"
#include "Types.hpp"
#include "render/Flags.hpp"
#include "render/RenderContext.hpp"
#include "vulkan-Check.hpp"

namespace R3 {

void CommandAllocator::create(RenderContext& ctx, uint32 queueIndex, CommandPoolMode mode) {
    m_device = ctx.device();
    m_mode   = mode;

    // Create the command pool
    const VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = mode,
        .queueFamilyIndex = queueIndex,
    };
    VK_CHECK(vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_pool));
}

void CommandAllocator::destroy() noexcept {
    if (m_pool != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_device, m_pool, nullptr);
        m_pool = VK_NULL_HANDLE;
    }
    m_device = VK_NULL_HANDLE;
}

VkCommandBuffer CommandAllocator::allocateBuffer(bool primary) {
    if (m_pool == VK_NULL_HANDLE) {
        throw Exception(__FUNCTION__ " called on unitialized CommandAllocator");
    }

    const VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = m_pool,
        .level              = (VkCommandBufferLevel)(!primary), // 0 == primary, 1 == secondary
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    VK_CHECK(vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, &commandBuffer));

    return commandBuffer;
}

std::vector<VkCommandBuffer> CommandAllocator::allocateBuffers(uint32_t count, bool primary) {
    if (count == 0) {
        return {};
    }

    if (m_pool == VK_NULL_HANDLE) {
        throw Exception(__FUNCTION__ " called on unitialized CommandAllocator");
    }

    const VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = m_pool,
        .level              = (VkCommandBufferLevel)(!primary), // 0 == primary, 1 == secondary
        .commandBufferCount = count,
    };

    std::vector<VkCommandBuffer> commandBuffers(count);
    VK_CHECK(vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, commandBuffers.data()));

    return commandBuffers;
}

void CommandAllocator::freeBuffer(VkCommandBuffer commandBuffer) noexcept {
    if (commandBuffer != VK_NULL_HANDLE && m_pool != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(m_device, m_pool, 1, &commandBuffer);
    }
}

void CommandAllocator::freeBuffers(std::span<VkCommandBuffer> commandBuffers) noexcept {
    if (!commandBuffers.empty() && m_pool != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(m_device, m_pool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    }
}

void CommandAllocator::reset(bool releaseResources) {
    if (m_pool == VK_NULL_HANDLE) {
        throw Exception("Command pool not created");
    }
    VK_CHECK(vkResetCommandPool(m_device, m_pool, (VkCommandPoolResetFlags)releaseResources));
}

void CommandAllocator::executeImmediate(Queue& queue, std::function<void(VkCommandBuffer)> function) {
    if (queue.handle == VK_NULL_HANDLE) {
        throw Exception("No queue available for immediate execution");
    }

    // Allocate a temporary command buffer
    VkCommandBuffer commandBuffer = allocateBuffer();

    // Begin recording
    const VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };

    try {
        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

        // Execute user function
        function(commandBuffer);

        // End recording
        VK_CHECK(vkEndCommandBuffer(commandBuffer));

        // Submit and wait
        const VkSubmitInfo submitInfo = {
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext                = nullptr,
            .waitSemaphoreCount   = 0,
            .pWaitSemaphores      = nullptr,
            .pWaitDstStageMask    = nullptr,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &commandBuffer,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores    = nullptr,
        };
        VK_CHECK(vkQueueSubmit(queue.handle, 1, &submitInfo, VK_NULL_HANDLE));

        // Wait for completion
        VK_CHECK(vkQueueWaitIdle(queue.handle));
    } catch (const Exception& ex) {
        freeBuffer(commandBuffer);
        throw ex;
    }

    // Clean up
    freeBuffer(commandBuffer);
}

} // namespace R3

#endif // R3_VULKAN
