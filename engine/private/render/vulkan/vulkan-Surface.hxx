#pragma once

#if R3_VULKAN

#include "render/Window.hpp"
#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/**
 * @brief Vulkan Surface Specification
 */
struct SurfaceSpecification {
    const Instance& instance; /**< Valid Vulkan Instance */
    Window& window;           /**< Valid Window */
};

/**
 * @brief Vulkan Surface RAII wrapper
 */
class Surface : public VulkanObject<VkSurfaceKHR> {
public:
    DEFAULT_CONSTRUCT(Surface);
    NO_COPY(Surface);
    DEFAULT_MOVE(Surface);

    /**
     * @brief Create Vulkan Surface stored as VulkanObject::Handle
     *
     * Uses GLFW to create OS dependant Surface and stores VkInstance from spec
     *
     * @param spec
     */
    Surface(const SurfaceSpecification& spec);

    /**
     * @brief Destroy Surface using vkDestroySurfaceKHR
     * @note vkDestroySurfaceKHR uses stored m_instance so it must be valid
     */
    ~Surface();

private:
    VkInstance m_instance = VK_NULL_HANDLE;
};

} // namespace R3::vulkan

#endif // R3_VULKAN