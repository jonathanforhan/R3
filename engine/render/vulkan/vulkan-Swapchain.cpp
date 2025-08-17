#if R3_VULKAN

#include "render/Swapchain.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <format>
#include <system_error>
#include <vector>
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>
#include <Exception.hpp>
#include "Types.hpp"
#include "render/RenderContext.hpp"
#include "render/Window.hpp"

#include "Log.hpp"

namespace R3 {

Swapchain::Swapchain(Window& window, RenderContext& ctx) {
    m_device = ctx.device();

    int width, height;
    glfwGetFramebufferSize(window.glfw(), &width, &height);

    uint32 iGraphics = ctx.graphicsQueue().index;
    uint32 iPresent  = ctx.presentQueue().index;

    /* RenderContext gives special access to private members during Swapchain::create */
    auto result = vkb::SwapchainBuilder(ctx.physicalDevice(), ctx.device(), ctx.surface(), iGraphics, iPresent)
                      .set_desired_format({VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                      .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR) // prefer triple buffering
                      .add_fallback_present_mode(VK_PRESENT_MODE_FIFO_KHR)   // guaranteed fallback
                      .set_desired_extent(width, height)
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

Swapchain::~Swapchain() noexcept {
    if (m_device != VK_NULL_HANDLE) {
        for (VkImageView imageView : m_imageViews) {
            vkDestroyImageView(m_device, imageView, nullptr);
        }
        m_images.clear();
        m_imageViews.clear();

        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }
    m_device = VK_NULL_HANDLE;
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
        .pSwapchains        = &m_swapchain.get(),
        .pImageIndices      = &imageIndex,
        .pResults           = nullptr,
    };
    return vkQueuePresentKHR(presentQueue, &presentInfo);
}

} // namespace R3

#endif // R3_VULKAN
