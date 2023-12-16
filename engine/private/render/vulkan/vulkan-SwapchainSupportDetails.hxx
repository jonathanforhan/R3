#if R3_VULKAN
#pragma once

// clang-format off
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
// clang-format on
#include "api/Types.hpp"
#include "render/RenderSpecification.hpp"

namespace R3::vulkan {

struct SwapchainSupportDetails {
private:
    SwapchainSupportDetails() = default;

public:
    vk::SurfaceCapabilitiesKHR capabilities = {};
    std::vector<vk::SurfaceFormatKHR> surfaceFormats;
    std::vector<vk::PresentModeKHR> presentModes;

    bool isValid() const { return !surfaceFormats.empty() && !presentModes.empty(); }

    [[nodiscard]] static SwapchainSupportDetails query(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);
    std::tuple<Format, ColorSpace> optimalSurfaceFormat() const;
    PresentMode optimalPresentMode() const;
    uvec2 optimalExtent(GLFWwindow* window) const;
};

} // namespace R3::vulkan

#endif // R3_VULKAN
