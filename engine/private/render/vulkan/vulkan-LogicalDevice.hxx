/**
 * @file vulkan-LogicalDevice.hxx
 * @copyright GNU Public License
 */

#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-Queue.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/**
 * Vulkan LogicalDevice Specification.
 */
struct LogicalDeviceSpecification {
    const Instance& instance;             /**< Valid Instance. */
    const Surface& surface;               /**< Valid Surface. */
    const PhysicalDevice& physicalDevice; /**< Valid PhysicalDevice. */
};

/**
 * Vulkan LogicalDevice RAII wrapper.
 * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDevice.html
 */
class LogicalDevice : public VulkanObject<VkDevice> {
public:
    DEFAULT_CONSTRUCT(LogicalDevice);
    NO_COPY(LogicalDevice);
    DEFAULT_MOVE(LogicalDevice);

    /**
     * LogicalDevice is the software abstraction for the PhysicalDevice.
     * LogicalDevice is used for many of the other create commands and holds
     * the Queues for the Device.
     * @param spec
     */
    LogicalDevice(const LogicalDeviceSpecification& spec);

    /**
     * Destroy device and automatically clean up queues.
     * @note All other Vulkan object relying on Device must be destroyed first
     */
    ~LogicalDevice();

    /**
     * Get graphics Queue.
     * @return Queue
     */
    const Queue& graphicsQueue() const { return m_graphicsQueue; }

    /**
     * Get presentation Queue.
     * @return Queue
     */
    const Queue& presentationQueue() const { return m_presentationQueue; }

private:
    Queue m_graphicsQueue;
    Queue m_presentationQueue;
};

} // namespace R3::vulkan

#endif // R3_VULKAN