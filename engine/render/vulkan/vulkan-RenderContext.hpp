#pragma once

#include <span>
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>
#include "Class.hpp"
#include "Types.hpp"
#include "render/Window.hpp"
#include "vulkan-Handle.hpp"

#ifdef R3_DEBUG
#define R3_VALIDATION_LAYERS_ENABLED 1
#else
#define R3_VALIDATION_LAYERS_ENABLED 0
#endif

namespace R3::vulkan {

/// @brief RenderContext holds vulkan resources:
///   - Instance
///   - Surface
///   - Device
///   - PhysicalDevice
class RenderContext {
public:
    R3_CTOR_DEFAULT(RenderContext);
    R3_COPY_DELETE(RenderContext);
    R3_MOVE_DEFAULT(RenderContext);

    /// @brief Create new RenderContext
    /// @param window Must be a valid Window for surface creation
    explicit RenderContext(Window& window);

    /// Destroys the RenderContext
    ~RenderContext() noexcept;

    /// Wait on the host for the completion of outstanding queue operations for all queues in this context
    void waitIdle();

    /// Get the index of memory type which satisfies ```typeFilter``` and ```properties```
    uint32 queryDeviceMemoryTypeIndex(uint32 typeFilter, VkMemoryPropertyFlags properties) const;

    /// Get physical device depth format
    VkFormat queryDepthFormat() const noexcept;

    /// Get physical device supported format chosen from a list of formats
    VkFormat querySupportedFormat(std::span<const VkFormat> formats,
                                  VkImageTiling tiling,
                                  VkFormatFeatureFlags features) const noexcept;

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
    Handle<VkInstance> m_instance;
    Handle<VkDebugUtilsMessengerEXT> m_debug;
    Handle<VkSurfaceKHR> m_surface;
    Handle<VkPhysicalDevice> m_physicalDevice;
    Handle<VkDevice> m_device;
    Handle<VkQueue> m_graphicsQueue;
    Handle<VkQueue> m_presentQueue;
    Handle<VkQueue> m_computeQueue;
    uint32 m_graphicsQueueIndex = 0xFFFFFFFF;
    uint32 m_presentQueueIndex  = 0xFFFFFFFF;
    uint32 m_computeQueueIndex  = 0xFFFFFFFF;
};

} // namespace R3::vulkan
