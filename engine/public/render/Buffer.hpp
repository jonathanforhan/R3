#pragma once

#include "api/Types.hpp"
#include "render/DeviceMemory.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

/// @brief Spec for allocating a buffer of memory, either on CPU or GPU
struct BufferAllocateSpecification {
    const PhysicalDevice& physicalDevice; ///< @brief Valid physical device
    const LogicalDevice& logicalDevice;   ///< @brief Valid logical device
    usize size;                           ///< @brief Buffer size in bytes
    uint64 bufferFlags;                   ///< @brief Buffer usage flags
    uint64 memoryFlags;                   ///< @brief Memory property flags
};

/// @brief Spec for copying a buffer of memory, either on CPU or GPU
struct BufferCopySpecification {
    const LogicalDevice& logicalDevice;      ///< @brief Valid physical device
    const CommandPool& commandPool;          ///< @brief Valid logical device
    NativeRenderObject& buffer;              ///< @brief dst buffer
    const NativeRenderObject& stagingBuffer; ///< @brief src buffer
    usize size;                              ///< @brief buffer size
};

/// @brief Buffer containing both a buffer handle and a handle to the underlying device memory handle
class Buffer : public DeviceMemory {
protected:
    Buffer() = default;

    /// @brief Allocate a Buffer with given flags
    /// @param spec
    /// @return (Buffer object, DeviceMemory object)
    [[nodiscard]] static std::tuple<NativeRenderObject, NativeRenderObject> allocate(
        const BufferAllocateSpecification& spec);

    /// @brief Copy stagingBuffer to buffer
    /// @param spec
    void static copy(const BufferCopySpecification& spec);
};

} // namespace R3