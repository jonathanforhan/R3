#include "vulkan-FrameSync.hpp"

#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

FrameSync::FrameSync(RenderContext& ctx, uint32 maxFramesInFlight, usize swapchainImageCount) {
    m_device            = ctx.device();
    m_maxFramesInFlight = maxFramesInFlight;

    const VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    const VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    // create per-frame acquire semaphores

    m_imageAvailableSemaphores.resize(m_maxFramesInFlight);
    for (auto& sem : m_imageAvailableSemaphores) {
        VK_CHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &sem));
    }

    m_inFlightFences.resize(m_maxFramesInFlight);
    for (auto& fence : m_inFlightFences) {
        VK_CHECK(vkCreateFence(m_device, &fenceInfo, nullptr, &fence));
    }

    // create per-image render finished semaphores

    m_renderFinishedSemaphores.resize(swapchainImageCount);
    for (auto& sem : m_renderFinishedSemaphores) {
        VK_CHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &sem));
    }
}

FrameSync::~FrameSync() noexcept {
    if (m_device) {
        for (auto& sem : m_imageAvailableSemaphores) {
            vkDestroySemaphore(m_device, sem, nullptr);
        }
        m_imageAvailableSemaphores.clear();

        for (auto& fence : m_inFlightFences) {
            vkDestroyFence(m_device, fence, nullptr);
        }
        m_inFlightFences.clear();

        for (auto& sem : m_renderFinishedSemaphores) {
            vkDestroySemaphore(m_device, sem, nullptr);
        }
        m_renderFinishedSemaphores.clear();
    }
}

void FrameSync::recreateImageSync(RenderContext& ctx, usize newSwapchainImageCount) {
    for (auto& sem : m_renderFinishedSemaphores) {
        vkDestroySemaphore(m_device, sem, nullptr);
    }

    const VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    m_renderFinishedSemaphores.resize(newSwapchainImageCount);
    for (auto& sem : m_renderFinishedSemaphores) {
        VK_CHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &sem));
    }
}

void FrameSync::waitForCurrentFrame() {
    VK_CHECK(vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX));
}

void FrameSync::resetCurrentFrame() {
    VK_CHECK(vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]));
}

} // namespace R3::vulkan