#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/Queue.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

/// @brief Logical Device Specification
struct LogicalDeviceSpecification {
    const Instance* instance;             ///< @brief Valid non-null Instance
    const Surface* surface;               ///< @brief Valid non-null Surface
    const PhysicalDevice* physicalDevice; ///< @brief Valid non-null PhysicalDevice
};

/// @brief LogicalDevice Represents the Application view of the underlaying hardware
/// The logical device is used for most operations including creating the components
/// of the renderer and mapping memory
class LogicalDevice : public NativeRenderObject {
public:
    /// @brief Create a LogicalDevice and Queues
    /// @note The queues are both created and destroyed by the logical device, it owns them
    /// @param spec 
    void create(const LogicalDeviceSpecification& spec);

    /// @brief Free LogicalDevice and Queues
    void destroy();

    const Queue& graphicsQueue() const { return m_graphicsQueue; }         ///< @brief Query Graphics Queue
    const Queue& presentationQueue() const { return m_presentationQueue; } ///< @brief Query Presentation Queue

private:
    LogicalDeviceSpecification m_spec;
    Queue m_graphicsQueue;
    Queue m_presentationQueue;
};

} // namespace R3