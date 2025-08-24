#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "api/Types.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

class FrameSync {
public:
    R3_CTOR_DEFAULT(FrameSync);
    R3_COPY_DELETE(FrameSync);
    R3_MOVE_DEFAULT(FrameSync);

    FrameSync(RenderContext& ctx, uint32 maxFramesInFlight, usize swapchainImageCount);

    ~FrameSync() noexcept;

    void recreateImageSync(RenderContext& ctx, usize newSwapchainImageCount);

    void waitForCurrentFrame();

    void resetCurrentFrame();

    void advanceFrame() noexcept { m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight; }

    VkSemaphore& currentImageAvailableSemaphore() noexcept { return m_imageAvailableSemaphores[m_currentFrame]; }

    VkFence& currentFence() noexcept { return m_inFlightFences[m_currentFrame]; }

    VkSemaphore& renderFinishedSemaphore(usize imageIndex) noexcept { return m_renderFinishedSemaphores[imageIndex]; }

    uint32 currentFrameIndex() const noexcept { return m_currentFrame; };

private:
    Handle<VkDevice> m_device;
    uint32 m_maxFramesInFlight = 0;
    uint32 m_currentFrame      = 0;
    std::vector<VkSemaphore> m_imageAvailableSemaphores; // one per frame in flight
    std::vector<VkFence> m_inFlightFences;               // one per frame in flight
    std::vector<VkSemaphore> m_renderFinishedSemaphores; // one per swapchain image
};

} // namespace R3::vulkan