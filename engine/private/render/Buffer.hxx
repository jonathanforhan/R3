#pragma once

/// @file Buffer.hxx
/// Parent class used by buffers that allocate some memory on host or device

#include "render/DeviceMemory.hxx"
#include "render/RenderApi.hxx"

namespace R3 {

/// @brief Spec for allocating a buffer of memory, either on CPU or GPU
struct BufferAllocateSpecification {
    const PhysicalDevice& physicalDevice; ///< Physical Device
    const LogicalDevice& logicalDevice;   ///< Logical Device
    usize size;                           ///< Buffer size in bytes
    BufferUsage::Flags bufferFlags;       ///< Buffer usage flags
    MemoryProperty::Flags memoryFlags;    ///< Memory property flags
};

/// @brief Spec for copying a buffer of memory, either on CPU or GPU
struct BufferCopySpecification {
    const LogicalDevice& logicalDevice;      ///< LogicalDevice
    const CommandBuffer& commandBuffer;      ///< CommandBuffer, a pool, used for one-time-command
    NativeRenderObject& buffer;              ///< Desination Buffer
    const NativeRenderObject& stagingBuffer; ///< Source Buffer
    usize size;                              ///< Buffer size in bytes
};

/// @brief Buffer abstraction, Contains a Buffer Handle and a DeviceMemory handle,
/// The Buffer handle is typically used for manipulation while the DeviceMemory handle is for allocation details
class Buffer : public DeviceMemory {
protected:
    DEFAULT_CONSTRUCT(Buffer);
    NO_COPY(Buffer);
    DEFAULT_MOVE(Buffer);

    /// @brief Allocate a Buffer with given flags
    /// @param spec
    /// @return (Buffer object, DeviceMemory object)
    [[nodiscard]] static auto allocate(const BufferAllocateSpecification& spec)
        -> std::tuple<NativeRenderObject, NativeRenderObject>;

    /// @brief Copy stagingBuffer to buffer
    /// @param spec
    void static copy(const BufferCopySpecification& spec);
};

} // namespace R3