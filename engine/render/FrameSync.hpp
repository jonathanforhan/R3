#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"

namespace R3 {

class RenderContext;

class FrameSync {
public:
    void create(RenderContext& ctx, uint32 maxFramesInFlight, usize swapchainImageCount);

    void destroy() noexcept;

    void recreateImageSync(RenderContext& ctx, usize newSwapchainImageCount);

    void waitForCurrentFrame();

    void resetCurrentFrame();

    void advanceFrame() noexcept { m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight; }

    VkSemaphore& currentImageAvailableSemaphore() noexcept { return m_imageAvailableSemaphores[m_currentFrame]; }

    VkFence& currentFence() noexcept { return m_inFlightFences[m_currentFrame]; }

    VkSemaphore& renderFinishedSemaphore(usize imageIndex) noexcept { return m_renderFinishedSemaphores[imageIndex]; }

    uint32 currentFrameIndex() const noexcept { return m_currentFrame; };

private:
    VkDevice m_device = VK_NULL_HANDLE;
    uint32 m_maxFramesInFlight;
    uint32 m_currentFrame = 0;
    std::vector<VkSemaphore> m_imageAvailableSemaphores; // one per frame in flight
    std::vector<VkFence> m_inFlightFences;               // one per frame in flight
    std::vector<VkSemaphore> m_renderFinishedSemaphores; // one per swapchain image
};

} // namespace R3