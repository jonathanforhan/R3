#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Class.hpp"
#include "Types.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

class Swapchain {
public:
    R3_CTOR_DEFAULT(Swapchain);
    R3_COPY_DELETE(Swapchain);
    R3_MOVE_DEFAULT(Swapchain);

    Swapchain(RenderContext& ctx, ivec2 framebufferSize);

    Swapchain(RenderContext& ctx, int32 framebufferWidth, int32 framebufferHeight);

    ~Swapchain() noexcept;

    void recreate(RenderContext& ctx, ivec2 framebufferSize);

    void recreate(RenderContext& ctx, int32 framebufferWidth, int32 framebufferHeight);

    VkFormat format() const noexcept { return m_format; }

    VkPresentModeKHR presentMode() const noexcept { return m_presentMode; }

    VkExtent2D extent() const noexcept { return m_extent; }

    std::span<const VkImage> images() const noexcept { return m_images; }

    std::span<const VkImageView> imageViews() const noexcept { return m_imageViews; }

    VkResult acquireNextImage(VkSemaphore semaphore, uint32& imageIndex, uint64 timeout = UINT64_MAX) const noexcept;

    VkResult present(VkQueue presentQueue, VkSemaphore waitSemaphore, uint32 imageIndex) const noexcept;

private:
    void create(RenderContext& ctx, int32 framebufferWidth, int32 framebufferHeight);

private:
    Handle<VkDevice> m_device;
    Handle<VkSwapchainKHR> m_swapchain;
    VkFormat m_format              = VK_FORMAT_B8G8R8A8_SRGB;
    VkPresentModeKHR m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
    VkExtent2D m_extent            = {};
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
};

} // namespace R3::vulkan