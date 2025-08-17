#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Handle.hpp"
#include "Types.hpp"

namespace R3 {

class Window;
class RenderContext;

class Swapchain {
public:
    Swapchain() = default;

    Swapchain(Window& window, RenderContext& ctx);

    ~Swapchain() noexcept;

    Swapchain(const Swapchain&)            = delete;
    Swapchain& operator=(const Swapchain&) = delete;

    Swapchain(Swapchain&&) noexcept            = default;
    Swapchain& operator=(Swapchain&&) noexcept = default;

    VkFormat format() const noexcept { return m_format; }

    VkPresentModeKHR presentMode() const noexcept { return m_presentMode; }

    uvec2 extent() const noexcept { return uvec2{m_extent.width, m_extent.height}; }

    std::span<const VkImage> images() const noexcept { return m_images; }

    std::span<const VkImageView> imageViews() const noexcept { return m_imageViews; }

    VkResult acquireNextImage(VkSemaphore semaphore, uint32& imageIndex, uint64 timeout = UINT64_MAX) const noexcept;

    VkResult present(VkQueue presentQueue, VkSemaphore waitSemaphore, uint32 imageIndex) const noexcept;

private:
#if R3_VULKAN
    Handle<VkDevice> m_device;
    Handle<VkSwapchainKHR> m_swapchain;
    VkFormat m_format              = VK_FORMAT_B8G8R8A8_SRGB;
    VkPresentModeKHR m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
    VkExtent2D m_extent            = {};
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
#endif
};

} // namespace R3