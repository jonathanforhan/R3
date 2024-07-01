////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-Queue.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief GPU Queue types.
enum class QueueType {
    Graphics,     ///< Graphics Queue.
    Presentation, ///< Presentation Queue.
    Compute,      ///< Compute Queue.
};

/// @brief QueueFamilyIndices Specification.
/// Queue indices are indices into Queues on the PhysicalDevice.
struct QueueFamilyIndices {
private:
    DEFAULT_CONSTRUCT(QueueFamilyIndices);

public:
    uint32 graphics     = undefined; ///< graphics queue index.
    uint32 presentation = undefined; ///< presentation queue index.

    /// @brief Check that all queue indices are valid.
    /// @return Validity
    constexpr bool valid() const { return graphics != undefined && presentation != undefined; }

    /// @brief QueueFamilyIndices is not publicly constructable, must be queried.
    /// @param physicalDevice Valid PhysicalDevice
    /// @param surface Valid Surface
    /// @return QueueFamilyIndices with graphics and presentation queues | InitializationFailure
    /// @{
    static Result<QueueFamilyIndices> query(const PhysicalDevice& physicalDevice, const Surface& surface);
    static Result<QueueFamilyIndices> query(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    /// @}
};

/// @brief Vulkan Queue Specification.
struct QueueSpecification {
    const LogicalDevice& device;     ///< Valid LogicalDevice used to query Queue.
    QueueType queueType = {};        ///< Type of Queue to acquire.
    uint32 queueIndex   = undefined; ///< Index of queue on PhsycialDevice.
};

/// @brief Vulkan Queue RAII wrapper.
/// Every operation in Vulkan is first submitting to a Queue for synchronization, this class handles the acquisition of
/// the Queue and holds Queue data like type and index.
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkQueue.html
class Queue : public VulkanObject<VkQueue> {
public:
    /// @brief Aquire queue from device.
    /// @note spec.queueIndex MUST be one the queues created by spec.device
    /// @param spec
    void acquire(const QueueSpecification& spec);

    /// @return Queue type
    constexpr QueueType type() const { return m_queueType; }

    /// @return Queue index
    constexpr uint32 index() const { return m_queueIndex; }

private:
    QueueType m_queueType = {};
    uint32 m_queueIndex   = undefined;
};

} // namespace R3::vulkan

#endif // R3_VULKAN