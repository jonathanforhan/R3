////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-LogicalDevice.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-Queue.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief Vulkan LogicalDevice Specification.
struct LogicalDeviceSpecification {
    const Instance& instance;             ///< Valid Instance.
    const Surface& surface;               ///< Valid Surface.
    const PhysicalDevice& physicalDevice; ///< Valid PhysicalDevice.
};

/// @brief Vulkan LogicalDevice RAII wrapper.
/// LogicalDevice is needed for many operations throughout the Renderer. The LogicalDevice queries and stores Queues on
/// creatation. Queues are automatically freed by Vulkan on vkDeviceDestroy.
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDevice.html
class LogicalDevice : public VulkanObject<VkDevice> {
public:
    DEFAULT_CONSTRUCT(LogicalDevice);
    NO_COPY(LogicalDevice);
    DEFAULT_MOVE(LogicalDevice);

    /// @brief Create LogicalDevice, query and assign Queues.
    /// @param spec
    LogicalDevice(const LogicalDeviceSpecification& spec);

    /// @brief Destroy device and automatically clean up queues.
    /// @note All other Vulkan object relying on Device must be destroyed first
    ~LogicalDevice();

    /// @return Const graphics Queue
    constexpr const Queue& graphicsQueue() const { return m_graphicsQueue; }

    /// @return Graphics Queue
    constexpr Queue& graphicsQueue() { return m_graphicsQueue; }

    /// @return Const presentation Queue
    constexpr const Queue& presentationQueue() const { return m_presentationQueue; }

    /// @return Presentation Queue
    constexpr Queue& presentationQueue() { return m_presentationQueue; }

private:
    Queue m_graphicsQueue;
    Queue m_presentationQueue;
};

} // namespace R3::vulkan

#endif // R3_VULKAN