#pragma once

/// @brief Provides means of querying and aquiring Queues

#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Queue Types used for Queue Creatation and Identification
enum class R3_API QueueType {
    Graphics,     ///< Graphics Queue
    Presentation, ///< Presentation Queue
    Compute,      ///< Compute Queue
};

/// @brief Queue Family Indice wrapper for the querying of queue indices
struct R3_API QueueFamilyIndices {
private:
    DEFAULT_CONSTRUCT(QueueFamilyIndices);

public:
    int32 graphics = -1;     ///< graphics queue index
    int32 presentation = -1; ///< presentation queue index

    /// @brief Query whether all queue indices are valid
    /// @return valid/invalid
    [[nodiscard]] constexpr bool isValid() const { return graphics >= 0 && presentation >= 0; }

    /// @brief QueueFamilyIndices is not publicly constructable, must be queried
    /// @param physicalDeviceHandle Valid non-null handle to Physical Device
    /// @param surfaceHandle Valid non-null handle to Surface
    /// @return QueueFamilyIndices
    [[nodiscard]] static QueueFamilyIndices query(NativeRenderObject&& physicalDeviceHandle,
                                                  NativeRenderObject&& surfaceHandle);
};

/// @brief Queue Specification
struct R3_API QueueSpecification {
    Ref<const LogicalDevice> logicalDevice; ///< LogicalDevice
    QueueType queueType;                    ///< type of queue to create
    uint32 queueIndex;                      ///< Index of queue as gotten through QueueFamilyIndices::query(...);
};

/// @brief Device Queue, Queues live on the hardware
/// Queues are obtains through query and then stored in LogicalDevice
class R3_API Queue : public NativeRenderObject {
public:
    /// @brief Queues live of the hardware and we only acquire them, not create them.
    /// Queues are released by LogicalDevice of when it is destroyed
    /// @param spec
    void acquire(const QueueSpecification& spec);

    /// @brief Query type of queue family
    /// @return type
    [[nodiscard]] constexpr QueueType type() const { return m_queueType; }

    /// @brief Query index of queue family
    /// @return index
    [[nodiscard]] constexpr uint32 index() const { return m_queueIndex; }

private:
    QueueType m_queueType;
    uint32 m_queueIndex;
};

} // namespace R3
