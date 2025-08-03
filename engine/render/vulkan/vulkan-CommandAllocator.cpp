#include "vulkan-CommandAllocator.hpp"

#include <cstdint>
#include <functional>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <Types.hpp>
#include "Exception.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3 {

void CommandAllocator::create(RenderContext& ctx, uint32 queueIndex, CommandPoolMode mode) noexcept(false) {
    m_device = ctx.device();
    m_mode   = mode;

    // Create the command pool
    VkCommandPoolCreateInfo commandPoolCreateInfo{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = getModeFlags(mode),
        .queueFamilyIndex = queueIndex,
    };

    VK_CHECK(vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_pool));
}

void CommandAllocator::destroy() noexcept(true) {
    if (m_pool != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_device, m_pool, nullptr);
        m_pool = VK_NULL_HANDLE;
    }
    m_device = VK_NULL_HANDLE;
}

VkCommandBuffer CommandAllocator::allocateBuffer(VkCommandBufferLevel level) noexcept(false) {
    if (m_pool == VK_NULL_HANDLE) {
        throw Exception(__FUNCTION__ " called on unitialized CommandAllocator");
    }

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = m_pool,
        .level              = level,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    VK_CHECK(vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, &commandBuffer));

    return commandBuffer;
}

std::vector<VkCommandBuffer> CommandAllocator::allocateBuffers(uint32_t count,
                                                               VkCommandBufferLevel level) noexcept(false) {
    if (count == 0) {
        return {};
    }

    if (m_pool == VK_NULL_HANDLE) {
        throw Exception(__FUNCTION__ " called on unitialized CommandAllocator");
    }

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = m_pool,
        .level              = level,
        .commandBufferCount = count,
    };

    std::vector<VkCommandBuffer> commandBuffers(count);
    VK_CHECK(vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, commandBuffers.data()));

    return commandBuffers;
}

void CommandAllocator::freeBuffer(VkCommandBuffer commandBuffer) noexcept(true) {
    if (commandBuffer != VK_NULL_HANDLE && m_pool != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(m_device, m_pool, 1, &commandBuffer);
    }
}

void CommandAllocator::freeBuffers(const std::vector<VkCommandBuffer>& commandBuffers) noexcept(true) {
    if (!commandBuffers.empty() && m_pool != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(m_device, m_pool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    }
}

void CommandAllocator::reset(VkCommandPoolResetFlags flags) noexcept(false) {
    if (m_pool == VK_NULL_HANDLE) {
        throw Exception("Command pool not created");
    }
    VK_CHECK(vkResetCommandPool(m_device, m_pool, flags));
}

void CommandAllocator::executeImmediate(Queue& queue, std::function<void(VkCommandBuffer)> function) noexcept(false) {
    if (queue.handle == VK_NULL_HANDLE) {
        throw Exception("No queue available for immediate execution");
    }

    // Allocate a temporary command buffer
    VkCommandBuffer commandBuffer = allocateBuffer();

    // Begin recording
    VkCommandBufferBeginInfo commandBufferBeginInfo{
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
        VkSubmitInfo submitInfo{
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

VkCommandPoolCreateFlags CommandAllocator::getModeFlags(CommandPoolMode mode) const noexcept(true) {
    switch (mode) {
        case CommandPoolMode::Protected:
            return VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
        case CommandPoolMode::Reset:
            return VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        case CommandPoolMode::Transient:
            return VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        default:
            return 0;
    }
}

} // namespace R3
