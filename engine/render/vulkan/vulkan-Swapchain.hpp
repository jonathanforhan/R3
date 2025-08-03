#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace R3 {

class RenderContext;
class Window;

class Swapchain {
public:
    void create(RenderContext& ctx, Window& window) noexcept(false);

    void recreate(RenderContext& ctx, Window& window) noexcept(false);

    void destroy() noexcept(true);

    VkFormat format() const noexcept(true) { return m_format; }

    VkPresentModeKHR presentMode() const noexcept(true) { return m_presentMode; }

    VkExtent2D extent() const noexcept(true) { return m_extent; }

    std::span<VkImage> images() noexcept(true) { return m_images; }
    std::span<const VkImage> images() const noexcept(true) { return m_images; }

    std::span<VkImageView> imageViews() noexcept(true) { return m_imageViews; }
    std::span<const VkImageView> imageViews() const noexcept(true) { return m_imageViews; }

    VkResult acquireNextImage(VkSemaphore semaphore, uint32_t& imageIndex, uint64_t timeout = UINT64_MAX) const
        noexcept(true);

    VkResult present(VkQueue presentQueue, VkSemaphore waitSemaphore, uint32_t imageIndex) const noexcept(true);

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