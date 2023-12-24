#pragma once

/// @file LogicalDevice.hpp

#include "render/Queue.hpp"
#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Logical Device Specification
struct LogicalDeviceSpecification {
    const Instance& instance;             ///< Instance
    const Surface& surface;               ///< Surface
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
};

/// @brief LogicalDevice represents the application view of the underlaying hardware
/// The logical device is used for most operations including creating the components
/// of the renderer and mapping memory
class LogicalDevice : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(LogicalDevice);
    NO_COPY(LogicalDevice);
    DEFAULT_MOVE(LogicalDevice);

    /// @brief Create a LogicalDevice and Queues
    /// @note The queues are both created and destroyed by the logical device, it owns them
    /// @param spec
    LogicalDevice(const LogicalDeviceSpecification& spec);

    /// @brief Free LogicalDevice and Queues
    ~LogicalDevice();

    const Queue& graphicsQueue() const { return m_graphicsQueue; }         ///< Query Graphics Queue
    const Queue& presentationQueue() const { return m_presentationQueue; } ///< Query Presentation Queue

private:
    Queue m_graphicsQueue;
    Queue m_presentationQueue;
};

} // namespace R3