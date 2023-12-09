#pragma once

#include "render/DeviceMemory.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

class Buffer : public DeviceMemory {
protected:
    Buffer() = default;

    /// @brief Allocate a Buffer with given flags
    /// @param logicalDevice
    /// @param physicalDevice 
    /// @param size size of buffer in bytes
    /// @param bufferFlags 
    /// @param memoryFlags 
    /// @return tuple<Buffer::Handle, DeviceMemory::Handle>
    [[nodiscard]] static std::tuple<Handle, Handle> allocate(const LogicalDevice& logicalDevice,
                                                             const PhysicalDevice& physicalDevice,
                                                             usize size,
                                                             uint64 bufferFlags,
                                                             uint64 memoryFlags);

    /// @brief Copy stagingBuffer to buffer
    /// @param[in] buffer dst buffer
    /// @param stagingBuffer src buffer
    /// @param size size bytes
    /// @param logicalDevice 
    /// @param commandPool
    void static copy(Handle buffer,
                     const Handle stagingBuffer,
                     usize size,
                     const LogicalDevice& logicalDevice,
                     const CommandPool& commandPool);
};

} // namespace R3