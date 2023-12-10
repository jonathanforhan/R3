#pragma once

#include "api/Types.hpp"
#include "render/DeviceMemory.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct BufferAllocateSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    usize size;
    uint64 bufferFlags;
    uint64 memoryFlags;
};

struct BufferCopySpecification {
    const LogicalDevice& logicalDevice;
    const CommandPool& commandPool;
    NativeRenderObject& buffer;
    const NativeRenderObject& stagingBuffer;
    usize size;
};

class Buffer : public DeviceMemory {
protected:
    Buffer() = default;

    /// @brief Allocate a Buffer with given flags
    /// @param spec 
    /// @return tuple<Buffer::Handle, DeviceMemory::Handle>
    [[nodiscard]] static std::tuple<NativeRenderObject, NativeRenderObject> allocate(
        const BufferAllocateSpecification& spec);

    /// @brief Copy stagingBuffer to buffer
    /// @param spec 
    void static copy(const BufferCopySpecification& spec);
                     
};

} // namespace R3