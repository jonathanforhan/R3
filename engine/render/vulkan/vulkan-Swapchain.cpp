#include "vulkan-Swapchain.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <format>
#include <system_error>
#include <vector>
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

Swapchain::Swapchain(RenderContext& ctx, ivec2 framebufferSize) {
    create(ctx, framebufferSize.x, framebufferSize.y);
}

Swapchain::Swapchain(RenderContext& ctx, int32 framebufferWidth, int32 framebufferHeight) {
    create(ctx, framebufferWidth, framebufferHeight);
}

Swapchain::~Swapchain() noexcept {
    if (m_device) {
        for (VkImageView imageView : m_imageViews) {
            vkDestroyImageView(m_device, imageView, nullptr);
        }
        m_images.clear();
        m_imageViews.clear();

        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    }
}

void Swapchain::recreate(RenderContext& ctx, ivec2 framebufferSize) {
    this->~Swapchain();
    create(ctx, framebufferSize.x, framebufferSize.y);
}

void Swapchain::recreate(RenderContext& ctx, int32 framebufferWidth, int32 framebufferHeight) {
    this->~Swapchain();
    create(ctx, framebufferWidth, framebufferHeight);
}

VkResult Swapchain::acquireNextImage(VkSemaphore semaphore, uint32& imageIndex, uint64 timeout) const noexcept {
    return vkAcquireNextImageKHR(m_device, m_swapchain, timeout, semaphore, VK_NULL_HANDLE, &imageIndex);
}

VkResult Swapchain::present(VkQueue presentQueue, VkSemaphore waitSemaphore, uint32 imageIndex) const noexcept {
    VkPresentInfoKHR presentInfo = {
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext              = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &waitSemaphore,
        .swapchainCount     = 1,
        .pSwapchains        = &*m_swapchain,
        .pImageIndices      = &imageIndex,
        .pResults           = nullptr,
    };
    return vkQueuePresentKHR(presentQueue, &presentInfo);
}

void Swapchain::create(RenderContext& ctx, int32 framebufferWidth, int32 framebufferHeight) {
    m_device = ctx.device();

    uint32 iGraphics = ctx.graphicsQueueIndex();
    uint32 iPresent  = ctx.presentQueueIndex();

    /* RenderContext gives special access to private members during Swapchain::create */
    auto result = vkb::SwapchainBuilder(ctx.physicalDevice(), ctx.device(), ctx.surface(), iGraphics, iPresent)
                      .set_desired_format({VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                      .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR) // prefer triple buffering
                      .add_fallback_present_mode(VK_PRESENT_MODE_FIFO_KHR)   // guaranteed fallback
                      .set_desired_extent(static_cast<uint32>(framebufferWidth), static_cast<uint32>(framebufferHeight))
                      .set_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                      .set_composite_alpha_flags(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
                      .set_clipped(true)
                      .build();

    if (!result) {
        throw Exception{std::format("failed to create VkSwapchain {}", result.error().value())};
    }
    vkb::Swapchain swapchain = result.value();

    m_swapchain   = swapchain.swapchain;
    m_images      = swapchain.get_images().value();
    m_imageViews  = swapchain.get_image_views().value();
    m_format      = swapchain.image_format;
    m_presentMode = swapchain.present_mode;
    m_extent      = swapchain.extent;
}

} // namespace R3::vulkan
