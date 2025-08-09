#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"

namespace R3 {

class RenderContext;
class Window;

class Swapchain {
public:
    void create(RenderContext& ctx, Window& window);

    void recreate(RenderContext& ctx, Window& window);

    void destroy() noexcept;

    VkFormat format() const noexcept { return m_format; }

    VkPresentModeKHR presentMode() const noexcept { return m_presentMode; }

    uvec2 extent() const noexcept { return uvec2{m_extent.width, m_extent.height}; }

    std::span<const VkImage> images() const noexcept { return m_images; }

    std::span<const VkImageView> imageViews() const noexcept { return m_imageViews; }

    VkResult acquireNextImage(VkSemaphore semaphore, uint32& imageIndex, uint64 timeout = UINT64_MAX) const noexcept;

    VkResult present(VkQueue presentQueue, VkSemaphore waitSemaphore, uint32 imageIndex) const noexcept;

private:
    VkDevice m_device              = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain     = VK_NULL_HANDLE;
    VkFormat m_format              = VK_FORMAT_B8G8R8A8_SRGB;
    VkPresentModeKHR m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
    VkExtent2D m_extent            = {};
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
};

} // namespace R3