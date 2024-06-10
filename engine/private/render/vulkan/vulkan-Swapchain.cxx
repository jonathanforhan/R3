#if R3_VULKAN

#include "vulkan-Swapchain.hxx"

#define GLFW_INCLUDE_VULKAN 1
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdint>
#include <tuple>
#include <utility>
#include "api/Assert.hpp"
#include "api/Log.hpp"
#include "api/Types.hpp"
#include "render/Window.hpp"
#include "vulkan-ColorBuffer.hxx"
#include "vulkan-DepthBuffer.hxx"
#include "vulkan-Framebuffer.hxx"
#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-Surface.hxx"

namespace R3::vulkan {

SwapchainSupportDetails SwapchainSupportDetails::query(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    SwapchainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    uint32 surfaceFormatsCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatsCount, nullptr);
    details.surfaceFormats.resize(surfaceFormatsCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatsCount, details.surfaceFormats.data());

    uint32 presentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, nullptr);
    details.presentModes.resize(presentModesCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, details.presentModes.data());

    return details;
}

std::tuple<VkFormat, VkColorSpaceKHR> SwapchainSupportDetails::optimalSurfaceFormat() const {
    // optimal formats we'd like to target
    static constexpr VkFormat optimalFormat            = VK_FORMAT_B8G8R8A8_SRGB;
    static constexpr VkColorSpaceKHR optimalColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    auto it = std::ranges::find_if(surfaceFormats, [](const VkSurfaceFormatKHR& surfaceFormat) {
        return surfaceFormat.format == optimalFormat && surfaceFormat.colorSpace == optimalColorSpace;
    });

    if (it != surfaceFormats.end()) {
        return std::tuple(optimalFormat, optimalColorSpace);
    }

    LOG(Warning, "surface format 32-bit SRGB is NOT available on this device, using sub-optimal format");
    return std::tuple(surfaceFormats.front().format, surfaceFormats.front().colorSpace);
}

VkPresentModeKHR SwapchainSupportDetails::optimalPresentMode() const {
    static constexpr VkPresentModeKHR optimalPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

    auto it = std::ranges::find_if(presentModes, [](auto& presentMode) { return presentMode == optimalPresentMode; });
    return VkPresentModeKHR(it != presentModes.end() ? optimalPresentMode : VK_PRESENT_MODE_FIFO_KHR);
}

VkExtent2D SwapchainSupportDetails::optimalExtent(Window& window) const {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }

    int32 width, height;
    window.size(&width, &height);

    VkExtent2D extent = {
        .width  = static_cast<uint32>(width),
        .height = static_cast<uint32>(height),
    };

    extent.width  = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return extent;
}

Swapchain::Swapchain(const SwapchainSpecification& spec)
    : m_device(spec.device) {
    // get details
    const auto swapchainSupportDetails       = SwapchainSupportDetails::query(spec.physicalDevice, spec.surface);
    const auto&& [surfaceFormat, colorSpace] = swapchainSupportDetails.optimalSurfaceFormat();

    m_surfaceFormat = surfaceFormat;
    m_colorSpace    = colorSpace;
    m_presentMode   = swapchainSupportDetails.optimalPresentMode();
    m_extent        = swapchainSupportDetails.optimalExtent(spec.window);

    swapchainSupportDetails.capabilities.minImageCount == swapchainSupportDetails.capabilities.maxImageCount
        ? m_imageCount = swapchainSupportDetails.capabilities.maxImageCount
        : m_imageCount = swapchainSupportDetails.capabilities.minImageCount + 1;

    const uint32 queueFamilyIndices[] = {
        spec.device.graphicsQueue().index(),
        spec.device.presentationQueue().index(),
    };
    const bool sameQueueFamily = queueFamilyIndices[0] == queueFamilyIndices[1];

    // create swapchain
    const VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = nullptr,
        .flags                 = {},
        .surface               = spec.surface,
        .minImageCount         = m_imageCount,
        .imageFormat           = m_surfaceFormat,
        .imageColorSpace       = m_colorSpace,
        .imageExtent           = m_extent,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = sameQueueFamily ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = 2,                  // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .pQueueFamilyIndices   = queueFamilyIndices, // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .preTransform          = swapchainSupportDetails.capabilities.currentTransform,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = m_presentMode,
        .clipped               = VK_TRUE,
        .oldSwapchain          = VK_NULL_HANDLE,
    };

    VkResult result = vkCreateSwapchainKHR(spec.device, &swapchainCreateInfo, nullptr, &m_handle);
    ENSURE(result == VK_SUCCESS);

    // acquire images from device
    uint32 imageCount;
    vkGetSwapchainImagesKHR(spec.device, m_handle, &imageCount, nullptr);

    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(spec.device, m_handle, &imageCount, m_images.data());

    // make the image views
    m_imageViews.resize(imageCount);

    for (uint32 i = 0; i < m_images.size(); i++) {
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext    = nullptr,
            .flags    = {},
            .image    = m_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format   = m_surfaceFormat,
            .components =
                {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        };

        VkResult result = vkCreateImageView(spec.device, &imageViewCreateInfo, nullptr, &m_imageViews[i]);
        ENSURE(result == VK_SUCCESS);
    }
}

Swapchain::~Swapchain() {
    for (VkImageView imageView : m_imageViews) {
        vkDestroyImageView(m_device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(m_device, m_handle, nullptr);
}

void Swapchain::recreate(const SwapchainRecreationSpecification& spec) {
    vkDeviceWaitIdle(m_device);

    // query
    auto swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(spec.physicalDevice, spec.surface);
    m_extent                     = swapchainSupportDetails.optimalExtent(spec.window);

    // if extent == 0 -> we're minimized -> wait idle until maximized
    while (m_extent.width == 0 || m_extent.height == 0) {
        glfwWaitEvents();
        swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(spec.physicalDevice, spec.surface);
        m_extent                = swapchainSupportDetails.optimalExtent(spec.window);
    }

    const uint32 queueFamilyIndices[] = {
        spec.device.graphicsQueue().index(),
        spec.device.presentationQueue().index(),
    };
    const bool sameQueueFamily = queueFamilyIndices[0] == queueFamilyIndices[1];

    // store old
    const VkSwapchainKHR oldSwapchain = m_handle;

    const VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = nullptr,
        .flags                 = {},
        .surface               = spec.surface,
        .minImageCount         = m_imageCount,
        .imageFormat           = m_surfaceFormat,
        .imageColorSpace       = m_colorSpace,
        .imageExtent           = m_extent,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = sameQueueFamily ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = 2,                  // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .pQueueFamilyIndices   = queueFamilyIndices, // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .preTransform          = swapchainSupportDetails.capabilities.currentTransform,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = m_presentMode,
        .clipped               = VK_TRUE,
        .oldSwapchain          = oldSwapchain,
    };

    vkCreateSwapchainKHR(spec.device, &swapchainCreateInfo, nullptr, &m_handle);
    vkDestroySwapchainKHR(m_device, oldSwapchain, nullptr);

    // recreate color buffer with new extent
    spec.colorBuffer.free();
    spec.colorBuffer = ColorBuffer({
        .physicalDevice = spec.physicalDevice,
        .device         = spec.device,
        .format         = spec.colorBuffer.format(),
        .extent         = m_extent,
        .sampleCount    = spec.colorBuffer.sampleCount(),
    });

    // recreate depth buffer with new extent
    spec.depthBuffer.free();
    spec.depthBuffer = DepthBuffer({
        .physicalDevice = spec.physicalDevice,
        .device         = spec.device,
        .extent         = m_extent,
        .sampleCount    = spec.depthBuffer.sampleCount(),
    });

    // acquire images from device (don't free previous, they are not allocated by us)
    uint32 imageCount;
    vkGetSwapchainImagesKHR(spec.device, m_handle, &imageCount, nullptr);

    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(spec.device, m_handle, &imageCount, m_images.data());

    for (VkImageView imageView : m_imageViews) {
        vkDestroyImageView(m_device, imageView, nullptr);
    }

    m_imageViews.resize(imageCount);
    spec.framebuffers.clear();

    for (uint32 i = 0; i < m_images.size(); i++) {
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext    = nullptr,
            .flags    = {},
            .image    = m_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format   = m_surfaceFormat,
            .components =
                {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        };

        VkResult result = vkCreateImageView(m_device, &imageViewCreateInfo, nullptr, &m_imageViews[i]);
        ENSURE(result == VK_SUCCESS);

        VkImageView attachments[] = {
            spec.colorBuffer.imageView(),
            spec.depthBuffer.imageView(),
            m_imageViews[i],
        };

        spec.framebuffers.emplace_back(FramebufferSpecification{
            .device      = spec.device,
            .renderPass  = spec.renderPass,
            .attachments = attachments,
            .extent      = m_extent,
        });
    }
}

} // namespace R3::vulkan

#endif // R3_VULKAN
