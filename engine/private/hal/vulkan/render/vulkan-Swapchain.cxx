#if R3_VULKAN

#include "render/Swapchain.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Surface.hpp"
#include "render/Window.hpp"
#include "vulkan-SwapchainSupportDetails.hxx"

namespace R3 {

void Swapchain::create(const SwapchainSpecification& spec) {
    CHECK(spec.physicalDevice != nullptr);
    CHECK(spec.surface != nullptr);
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.window != nullptr);
    m_spec = spec;

    auto swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(
        m_spec.physicalDevice->handle<VkPhysicalDevice>(), m_spec.surface->handle<VkSurfaceKHR>());

    auto [m_surfaceFormat, m_colorSpace] = swapchainSupportDetails.optimalSurfaceFormat();
    m_extent2D = swapchainSupportDetails.optimalExtent(m_spec.window->handle<GLFWwindow*>());

    swapchainSupportDetails.capabilities.minImageCount == swapchainSupportDetails.capabilities.maxImageCount
        ? m_imageCount = swapchainSupportDetails.capabilities.maxImageCount
        : m_imageCount = swapchainSupportDetails.capabilities.minImageCount + 1;

    uint32 queueFamilyIndices[] = {
        m_spec.logicalDevice->graphicsQueue().index(),
        m_spec.logicalDevice->presentattionQueue().index(),
    };
    bool sameQueueFamily = queueFamilyIndices[0] == queueFamilyIndices[1];

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0U,
        .surface = m_spec.surface->handle<VkSurfaceKHR>(),
        .minImageCount = m_imageCount,
        .imageFormat = (VkFormat)m_surfaceFormat,
        .imageColorSpace = (VkColorSpaceKHR)m_colorSpace,
        .imageExtent = VkExtent2D(m_extent2D.x, m_extent2D.y),
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = sameQueueFamily ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = 2,                // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .pQueueFamilyIndices = queueFamilyIndices, // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .preTransform = swapchainSupportDetails.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = (VkPresentModeKHR)m_presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    ENSURE(vkCreateSwapchainKHR(
               m_spec.logicalDevice->handle<VkDevice>(), &swapchainCreateInfo, nullptr, handlePtr<VkSwapchainKHR*>()) ==
           VK_SUCCESS);

    m_images = Image::acquireImages({
        .logicalDevice = m_spec.logicalDevice,
        .swapchain = this,
    });

    m_imageViews.resize(m_images.size());
    for (usize i = 0; i < m_images.size(); i++) {
        m_imageViews[i].create({
            .logicalDevice = m_spec.logicalDevice,
            .swapchain = this,
            .image = &m_images[i],
        });
    }
}

void Swapchain::destroy() {
    for (auto& imageView : m_imageViews) {
        imageView.destroy();
    }
    vkDestroySwapchainKHR(m_spec.logicalDevice->handle<VkDevice>(), handle<VkSwapchainKHR>(), nullptr);
}

} // namespace R3

#endif // R3_VULKAN