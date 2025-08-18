#pragma once

#if R3_VULKAN

#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"
#include "render/Window.hpp"

#ifdef R3_DEBUG
#define R3_VALIDATION_LAYERS_ENABLED 1
#else
#define R3_VALIDATION_LAYERS_ENABLED 0
#endif

namespace R3::vulkan {

/**
 * @brief RenderContext holds vulkan resources:
 *   - Instance
 *   - Surface
 *   - Device
 *   - PhysicalDevice
 */
class RenderContext {
public:
    /**
     * @brief Create new RenderContext
     * @param window Must be a valid Window for surface creation
     */
    void create(Window& window);

    /// Destroys the RenderContext
    void destroy() noexcept;

    /// Wait on the host for the completion of outstanding queue operations for all queues in this context
    void waitIdle();

    /// get the index of memory type which satisfies ```typeFilter``` and ```properties```
    uint32 deviceMemoryTypeIndex(uint32 typeFilter, VkMemoryPropertyFlags properties) const;

    /// VkSurfaceKHR getter
    VkSurfaceKHR surface() const noexcept { return m_surface; }
    /// VkPhysicalDevice getter
    VkPhysicalDevice physicalDevice() const noexcept { return m_physicalDevice; }
    /// VkDevice getter
    VkDevice device() const noexcept { return m_device; }

    /// Graphics VkQueue getter
    VkQueue graphicsQueue() const noexcept { return m_graphicsQueue; }
    /// Present VkQueue getter
    VkQueue presentQueue() const noexcept { return m_presentQueue; }
    /// Compute VkQueue getter
    VkQueue computeQueue() const noexcept { return m_computeQueue; }

    /// Graphics queue family index getter
    uint32 graphicsQueueIndex() const noexcept { return m_graphicsQueueIndex; }
    /// Present queue family index getter
    uint32 presentQueueIndex() const noexcept { return m_presentQueueIndex; }
    /// Compute queue family index getter
    uint32 computeQueueIndex() const noexcept { return m_computeQueueIndex; }

private:
    static vkb::Instance createInstance();
    static VkSurfaceKHR createSurface(Window& window, VkInstance instance);
    static vkb::PhysicalDevice selectPhysicalDevice(const vkb::Instance& instance, VkSurfaceKHR surface);
    static vkb::Device createLogicalDevice(const vkb::PhysicalDevice& physicalDevice);
    static void setupQueue(const vkb::Device& device, vkb::QueueType queueType, VkQueue& queue, uint32& index);

private:
    VkInstance m_instance             = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debug  = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface            = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device                 = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue           = VK_NULL_HANDLE;
    VkQueue m_presentQueue            = VK_NULL_HANDLE;
    VkQueue m_computeQueue            = VK_NULL_HANDLE;
    uint32 m_graphicsQueueIndex       = 0xFFFFFFFF;
    uint32 m_presentQueueIndex        = 0xFFFFFFFF;
    uint32 m_computeQueueIndex        = 0xFFFFFFFF;
};

#if R3_VALIDATION_LAYERS_ENABLED

VKAPI_ATTR VkBool32 VKAPI_CALL validationDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                       void* pUserData);

#endif

} // namespace R3::vulkan

#endif // R3_VULKAN