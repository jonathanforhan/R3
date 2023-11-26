#if R3_VULKAN

// clang-format off
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <tuple>
#include <vector>
#include "api/Types.hpp"
#include "render/RenderSpecification.hpp"

namespace R3::vulkan {

struct SwapchainSupportDetails {
private:
    SwapchainSupportDetails() = default;

public:
    VkSurfaceCapabilitiesKHR capabilities = {};
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> presentModes;

    bool isValid() const { return !surfaceFormats.empty() && !presentModes.empty(); }

    [[nodiscard]] static SwapchainSupportDetails query(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    std::tuple<Format, ColorSpace> optimalSurfaceFormat() const;
    PresentMode optimalPresentMode() const;
    uvec2 optimalExtent(GLFWwindow* window) const;
};

} // namespace R3::vulkan

#endif // R3_VULKAN
