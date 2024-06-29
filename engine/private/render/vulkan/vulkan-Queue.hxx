/**
 * @file vulkan-Queue.hxx
 * @copyright GNU Public License
 */

#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Types.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/**
 * GPU Queue types.
 */
enum class QueueType {
    Graphics,     /**< Graphics Queue. */
    Presentation, /**< Presentation Queue. */
    Compute,      /**< Compute Queue. */
};

/**
 * QueueFamilyIndices Specification.
 * Queue indices are indices into Queues on the PhysicalDevice.
 */
struct QueueFamilyIndices {
private:
    DEFAULT_CONSTRUCT(QueueFamilyIndices);

public:
    uint32 graphics     = undefined; /**< graphics queue index. */
    uint32 presentation = undefined; /**< presentation queue index. */

    /**
     * Check that all queue indices are valid.
     * @return Validity
     */
    constexpr bool isValid() const { return graphics != undefined && presentation != undefined; }

    /**
     * QueueFamilyIndices is not publicly constructable, must be queried.
     * @param physicalDevice Valid VkPhysicalDevice
     * @param surface Valid VkSurfaceKHR
     * @return QueueFamilyIndices with graphics and presentation queues
     */
    static QueueFamilyIndices query(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
};

/**
 * Vulkan Queue Specification.
 */
struct QueueSpecification {
    const LogicalDevice& device;     /**< Valid LogicalDevice used to query Queue. */
    QueueType queueType = {};        /**< Type of Queue to acquire. */
    uint32 queueIndex   = undefined; /**< Index of queue on PhsycialDevice. */
};

/**
 * Vulkan Queue RAII wrapper.
 * Every operation in Vulkan is first submitting to a Queue for synchronization,
 * this class handles the acquisition of the Queue and holds Queue data like
 * type and index.
 * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkQueue.html
 */
class Queue : public VulkanObject<VkQueue> {
public:
    /**
     * Aquire queue from device.
     * @param spec
     */
    void acquire(const QueueSpecification& spec);

    /**
     * @return Queue type
     */
    constexpr QueueType type() const { return m_queueType; }

    /**
     * @return Queue index
     */
    constexpr uint32 index() const { return m_queueIndex; }

private:
    QueueType m_queueType = {};
    uint32 m_queueIndex   = undefined;
};

} // namespace R3::vulkan

#endif // R3_VULKAN