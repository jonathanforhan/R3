#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/Queue.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

/// @brief Logical Device Specification
struct LogicalDeviceSpecification {
    Ref<const Instance> instance;             ///< @brief Valid Instance
    Ref<const Surface> surface;               ///< @brief Valid Surface
    Ref<const PhysicalDevice> physicalDevice; ///< @brief Valid PhysicalDevice
};

/// @brief LogicalDevice represents the application view of the underlaying hardware
/// The logical device is used for most operations including creating the components
/// of the renderer and mapping memory
class LogicalDevice : public NativeRenderObject {
public:
    LogicalDevice() = default;

    /// @brief Create a LogicalDevice and Queues
    /// @note The queues are both created and destroyed by the logical device, it owns them
    /// @param spec
    LogicalDevice(const LogicalDeviceSpecification& spec);

    LogicalDevice(LogicalDevice&&) noexcept = default;
    LogicalDevice& operator=(LogicalDevice&&) noexcept = default;

    /// @brief Free LogicalDevice and Queues
    ~LogicalDevice();

    const Queue& graphicsQueue() const { return m_graphicsQueue; }         ///< @brief Query Graphics Queue
    const Queue& presentationQueue() const { return m_presentationQueue; } ///< @brief Query Presentation Queue

private:
    Queue m_graphicsQueue;
    Queue m_presentationQueue;
};

} // namespace R3