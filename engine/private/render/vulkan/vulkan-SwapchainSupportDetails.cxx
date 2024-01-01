#if R3_VULKAN

#include "vulkan-SwapchainSupportDetails.hxx"
#include <algorithm>
#include <ranges>
#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "api/Log.hpp"

namespace R3::vulkan {

SwapchainSupportDetails SwapchainSupportDetails::query(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {
    SwapchainSupportDetails swapchainSupportDetails;
    swapchainSupportDetails.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    swapchainSupportDetails.surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
    swapchainSupportDetails.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
    return swapchainSupportDetails;
}

std::tuple<Format, ColorSpace> SwapchainSupportDetails::optimalSurfaceFormat() const {
    static constexpr auto optimalFormat = vk::Format::eR8G8B8A8Srgb;
    static constexpr auto optimalColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;

    auto it = std::ranges::find_if(surfaceFormats, [](const auto& surfaceFormat) {
        return surfaceFormat.format == optimalFormat && surfaceFormat.colorSpace == optimalColorSpace;
    });

    if (it != surfaceFormats.end()) {
        return {Format(optimalFormat), ColorSpace(optimalColorSpace)};
    }

    LOG(Warning, "surface format 32-bit SRGB is NOT available on this device, using sub-optimal format");
    return {Format(surfaceFormats.front().format), ColorSpace(surfaceFormats.front().colorSpace)};
}

PresentMode SwapchainSupportDetails::optimalPresentMode() const {
    static constexpr auto optimalPresentMode = vk::PresentModeKHR::eMailbox;

    auto it = std::ranges::find_if(presentModes, [](auto& presentMode) { return presentMode == optimalPresentMode; });
    return PresentMode(it != presentModes.end() ? optimalPresentMode : vk::PresentModeKHR::eFifo);
}

uvec2 SwapchainSupportDetails::optimalExtent(GLFWwindow* window) const {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return uvec2(capabilities.currentExtent.width, capabilities.currentExtent.height);
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    vk::Extent2D extent = {
        .width = static_cast<uint32>(width),
        .height = static_cast<uint32>(height),
    };

    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return uvec2(extent.width, extent.height);
}

bool SwapchainSupportDetails::isMinimized(GLFWwindow* window) const {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return !(width | height);
}

} // namespace R3::vulkan

#endif // R3_VULKAN
