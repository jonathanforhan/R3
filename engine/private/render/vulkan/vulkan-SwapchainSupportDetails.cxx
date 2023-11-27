#if R3_VULKAN

#include "vulkan-SwapchainSupportDetails.hxx"
#include <algorithm>
#include "api/Check.hpp"
#include "api/Log.hpp"

namespace R3::vulkan {

SwapchainSupportDetails SwapchainSupportDetails::query(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    SwapchainSupportDetails swapchainSupportDetails;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapchainSupportDetails.capabilities);

    uint32 surfaceFormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
    CHECK(surfaceFormatCount != 0);

    swapchainSupportDetails.surfaceFormats.resize(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice, surface, &surfaceFormatCount, swapchainSupportDetails.surfaceFormats.data());

    uint32 presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    CHECK(presentModeCount != 0);

    swapchainSupportDetails.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice, surface, &presentModeCount, swapchainSupportDetails.presentModes.data());

    return swapchainSupportDetails;
}

std::tuple<Format, ColorSpace> SwapchainSupportDetails::optimalSurfaceFormat() const {
    for (const auto& surfaceFormat : surfaceFormats) {
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return {
                (Format)surfaceFormat.format,
                (ColorSpace)surfaceFormat.colorSpace,
            };
        }
    }

    LOG(Warning, "surface format 32-bit SRGB is NOT available on this device, using sub-optimal format");

    return {
        (Format)surfaceFormats.front().format,
        (ColorSpace)surfaceFormats.front().colorSpace,
    };
}

PresentMode SwapchainSupportDetails::optimalPresentMode() const {
    for (const auto& presentMode : presentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return (PresentMode)presentMode;
        }
    }

    return (PresentMode)VK_PRESENT_MODE_FIFO_KHR;
}

uvec2 SwapchainSupportDetails::optimalExtent(GLFWwindow* window) const {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return uvec2(capabilities.currentExtent.width, capabilities.currentExtent.height);
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D extent = {
        .width = static_cast<uint32>(width),
        .height = static_cast<uint32>(height),
    };

    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return uvec2(extent.width, extent.height);
}

} // namespace R3::vulkan

#endif // R3_VULKAN