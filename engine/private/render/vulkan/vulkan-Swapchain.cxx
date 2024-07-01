#if R3_VULKAN

#include "vulkan-Swapchain.hxx"

#define GLFW_INCLUDE_VULKAN 1
#include <GLFW/glfw3.h>

#include <algorithm>
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <cstdint>
#include <expected>
#include <tuple>
#include <type_traits>
#include <utility>
#include "render/Window.hpp"
#include "vulkan-ColorBuffer.hxx"
#include "vulkan-DepthBuffer.hxx"
#include "vulkan-Framebuffer.hxx"
#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-Surface.hxx"

namespace R3::vulkan {

Result<SwapchainSupportDetails> SwapchainSupportDetails::query(const PhysicalDevice physicalDevice,
                                                               const Surface& surface) {
    return query(physicalDevice.vk(), surface.vk());
}

Result<SwapchainSupportDetails> SwapchainSupportDetails::query(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    SwapchainSupportDetails details;
    (void)vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    uint32 surfaceFormatsCount;
    (void)vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatsCount, nullptr);

    details.surfaceFormats.resize(surfaceFormatsCount);
    (void)vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice, surface, &surfaceFormatsCount, details.surfaceFormats.data());

    uint32 presentModesCount;
    (void)vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, nullptr);

    details.presentModes.resize(presentModesCount);
    (void)vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice, surface, &presentModesCount, details.presentModes.data());

    if (details.valid()) {
        return details;
    }

    R3_LOG(Error, "invalid swapchain details");
    return std::unexpected(Error::InitializationFailure);
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

    R3_LOG(Warning, "surface format 32-bit SRGB is NOT available on this device, using sub-optimal format");
    return std::tuple(surfaceFormats.front().format, surfaceFormats.front().colorSpace);
}

VkPresentModeKHR SwapchainSupportDetails::optimalPresentMode() const {
    static constexpr VkPresentModeKHR optimalPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

    auto it = std::ranges::find_if(presentModes, [](auto& presentMode) { return presentMode == optimalPresentMode; });
    return it != presentModes.end() ? optimalPresentMode : VK_PRESENT_MODE_FIFO_KHR;
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

Result<Swapchain> Swapchain::create(const SwapchainSpecification& spec) {
    Swapchain self;
    self.m_device = spec.device.vk();

    // get details
    const auto swapchainSupportDetails = SwapchainSupportDetails::query(spec.physicalDevice.vk(), spec.surface.vk());
    // R3_PROPAGATE(swapchainSupportDetails); should always be good

    const auto&& [surfaceFormat, colorSpace] = swapchainSupportDetails->optimalSurfaceFormat();

    self.m_surfaceFormat = surfaceFormat;
    self.m_colorSpace    = colorSpace;
    self.m_presentMode   = swapchainSupportDetails->optimalPresentMode();
    self.m_extent        = swapchainSupportDetails->optimalExtent(spec.window);

    self.m_imageCount = swapchainSupportDetails->capabilities.minImageCount + 1;
    if (swapchainSupportDetails->capabilities.maxImageCount > 0) {
        self.m_imageCount = std::min(self.m_imageCount, swapchainSupportDetails->capabilities.maxImageCount);
    }

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
        .surface               = spec.surface.vk(),
        .minImageCount         = self.m_imageCount,
        .imageFormat           = self.m_surfaceFormat,
        .imageColorSpace       = self.m_colorSpace,
        .imageExtent           = self.m_extent,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = sameQueueFamily ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = 2,                  // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .pQueueFamilyIndices   = queueFamilyIndices, // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .preTransform          = swapchainSupportDetails->capabilities.currentTransform,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = self.m_presentMode,
        .clipped               = VK_TRUE,
        .oldSwapchain          = VK_NULL_HANDLE,
    };

    VkResult result = vkCreateSwapchainKHR(spec.device.vk(), &swapchainCreateInfo, nullptr, &self.m_handle);
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "vkCreateSwapchainKHR failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    // acquire images from device
    uint32 imageCount;
    (void)vkGetSwapchainImagesKHR(spec.device.vk(), self.m_handle, &imageCount, nullptr);

    self.m_images.resize(imageCount);
    (void)vkGetSwapchainImagesKHR(spec.device.vk(), self.m_handle, &imageCount, self.m_images.data());

    // make the image views
    self.m_imageViews.resize(imageCount);

    for (uint32 i = 0; i < self.m_images.size(); i++) {
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext    = nullptr,
            .flags    = {},
            .image    = self.m_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format   = self.m_surfaceFormat,
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

        result = vkCreateImageView(spec.device.vk(), &imageViewCreateInfo, nullptr, &self.m_imageViews[i]);
        if (result != VK_SUCCESS) {
            R3_LOG(Error, "vkCreateImageView failure {}", (int)result);
            return std::unexpected(Error::InitializationFailure);
        }
    }

    return self;
}

Swapchain::~Swapchain() {
    for (VkImageView imageView : m_imageViews) {
        vkDestroyImageView(m_device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(m_device, m_handle, nullptr);
}

Result<void> Swapchain::recreate(const SwapchainRecreationSpecification& spec) {
    (void)vkDeviceWaitIdle(m_device);

    // query
    auto swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(spec.physicalDevice.vk(), spec.surface.vk());
    R3_PROPAGATE(swapchainSupportDetails);

    m_extent = swapchainSupportDetails->optimalExtent(spec.window);

    // if extent == 0 -> we're minimized -> wait idle until maximized
    while (m_extent.width == 0 || m_extent.height == 0) {
        glfwWaitEvents();
        swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(spec.physicalDevice.vk(), spec.surface.vk());
        m_extent                = swapchainSupportDetails->optimalExtent(spec.window);
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
        .surface               = spec.surface.vk(),
        .minImageCount         = m_imageCount,
        .imageFormat           = m_surfaceFormat,
        .imageColorSpace       = m_colorSpace,
        .imageExtent           = m_extent,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = sameQueueFamily ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = 2,                  // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .pQueueFamilyIndices   = queueFamilyIndices, // NOTE does nothing if VK_SHARING_MODE_EXCLUSIVE is true
        .preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = m_presentMode,
        .clipped               = VK_TRUE,
        .oldSwapchain          = oldSwapchain,
    };

    VkResult result = vkCreateSwapchainKHR(spec.device.vk(), &swapchainCreateInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "vkCreateSwapchainKHR failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    vkDestroySwapchainKHR(m_device, oldSwapchain, nullptr);

    // recreate color buffer with new extent
    Result<ColorBuffer> colorBuffer = ColorBuffer::create({
        .physicalDevice = spec.physicalDevice,
        .device         = spec.device,
        .format         = m_surfaceFormat,
        .extent         = m_extent,
        .sampleCount    = spec.physicalDevice.sampleCount(),
    });
    R3_PROPAGATE(colorBuffer);

    spec.colorBuffer.free();
    spec.colorBuffer = std::move(colorBuffer.value());

    // recreate depth buffer with new extent
    Result<DepthBuffer> depthBuffer = DepthBuffer::create({
        .physicalDevice = spec.physicalDevice,
        .device         = spec.device,
        .extent         = m_extent,
        .sampleCount    = spec.physicalDevice.sampleCount(),
    });
    spec.depthBuffer.free();
    spec.depthBuffer = std::move(depthBuffer.value());

    // acquire images from device (don't free previous, they are not allocated by us)
    uint32 imageCount;
    (void)vkGetSwapchainImagesKHR(spec.device.vk(), m_handle, &imageCount, nullptr);

    m_images.resize(imageCount);
    (void)vkGetSwapchainImagesKHR(spec.device.vk(), m_handle, &imageCount, m_images.data());

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

        result = vkCreateImageView(m_device, &imageViewCreateInfo, nullptr, &m_imageViews[i]);
        if (result != VK_SUCCESS) {
            R3_LOG(Error, "vkCreateImageView failure {}", (int)result);
            return std::unexpected(Error::InitializationFailure);
        }

        VkImageView attachments[] = {
            spec.colorBuffer.imageView(),
            spec.depthBuffer.imageView(),
            m_imageViews[i],
        };

        Result<Framebuffer> framebuffer = Framebuffer::create({
            .device      = spec.device,
            .renderPass  = spec.renderPass,
            .attachments = attachments,
            .extent      = m_extent,
        });
        R3_PROPAGATE(framebuffer);

        spec.framebuffers.emplace_back(std::move(framebuffer.value()));
    }

    return Result<void>();
}

} // namespace R3::vulkan

#endif // R3_VULKAN
