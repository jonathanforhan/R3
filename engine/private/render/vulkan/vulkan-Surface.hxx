////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-Surface.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "render/Window.hpp"
#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Result.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief Vulkan Surface Specification.
struct SurfaceSpecification {
    const Instance& instance; ///< Valid Vulkan Instance.
    Window& window;           ///< Valid Window.
};

/// @brief Vulkan Surface RAII wrapper.
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSurfaceKHR.html
class Surface : public VulkanObject<VkSurfaceKHR> {
public:
    DEFAULT_CONSTRUCT(Surface);
    NO_COPY(Surface);
    DEFAULT_MOVE(Surface);

    /// @brief Create Vulkan Window Surface.
    /// Uses GLFW to create OS dependant Surface and stores VkInstance from spec.
    /// @param spec
    /// @return Surface | InitializationFailure
    static Result<Surface> create(const SurfaceSpecification& spec);

    /// @brief Destroy Surface using vkDestroySurfaceKHR.
    /// @note vkDestroySurfaceKHR uses stored m_instance so it must be valid
    ~Surface();

private:
    VkInstance m_instance = VK_NULL_HANDLE;
};

} // namespace R3::vulkan

#endif // R3_VULKAN