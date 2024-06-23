#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Types.hpp>
#include <mutex>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/**
 * @brief Internal GPU Queue types
 */
enum class QueueType {
    Graphics,     /**< Graphics Queue */
    Presentation, /**< Presentation Queue */
    Compute,      /**< Compute Queue */
};

/**
 * @brief QueueFamilyIndices Specification
 *
 * Queue indices are indices into Queues on the PhysicalDevice
 */
struct QueueFamilyIndices {
private:
    DEFAULT_CONSTRUCT(QueueFamilyIndices);

public:
    uint32 graphics     = undefined; /**< graphics queue index */
    uint32 presentation = undefined; /**< presentation queue index */

    /**
     * @brief Check is all queue indices are valid
     * @return valid?
     */
    constexpr bool isValid() const { return graphics != undefined && presentation != undefined; }

    /**
     * @brief QueueFamilyIndices is not publicly constructable, must be queried
     * @param physicalDevice Valid VkPhysicalDevice
     * @param surface Valid VkSurfaceKHR
     * @return Constructed QueueFamilyIndices with graphics and present queues
     */
    static QueueFamilyIndices query(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
};

/**
 * @brief Vulkan Queue Specification
 */
struct QueueSpecification {
    const LogicalDevice& device;     /**< Valid Vulkan Device to get Queue from */
    QueueType queueType = {};        /**< Type of Queue to acquire */
    uint32 queueIndex   = undefined; /**< Index of queue on PhsycialDevice */
};

/**
 * @brief Vulkan Queue RAII wrapper
 *
 * Every operation in Vulkan is first submitting to a Queue for synchronization,
 * this class provides locking mechanisms for that. Queue is created and
 * automatically freed by the LogicalDevice.
 */
class Queue : public VulkanObject<VkQueue> {
public:
    /**
     * @brief Aquire queue from device, sets up mutex for locking
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

    std::scoped_lock<std::mutex> scopedLock() { return std::scoped_lock(*m_mutex); }

private:
    QueueType m_queueType = {};
    uint32 m_queueIndex   = undefined;
    std::mutex* m_mutex   = nullptr; // points to static mutex in source file
};

} // namespace R3::vulkan

#endif // R3_VULKAN