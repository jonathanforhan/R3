#pragma once

#include "RenderHandle.hpp"
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/MovableHandle.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

/// @brief Defines usage flags for buffer resources, specifying their intended operations and memory properties.
struct R3_API BufferUsage {
    enum : flag_t {
        TransferSrc     = 0x0000'0001,
        TransferDst     = 0x0000'0002,
        UniformTexel    = 0x0000'0004,
        StorageTexel    = 0x0000'0008,
        Uniform         = 0x0000'0010,
        Storage         = 0x0000'0020,
        Index           = 0x0000'0040,
        Vertex          = 0x0000'0080,
        DeviceLocal     = 0x0001'0000,
        HostVisible     = 0x0002'0000,
        HostCoherent    = 0x0004'0000,
        LazilyAllocated = 0x0010'0000,
        MapOnCreation   = 0x8000'0000,
        HostStaging     = TransferSrc | HostVisible | HostCoherent | MapOnCreation,
        HostUniform     = Uniform | HostVisible | HostCoherent | MapOnCreation,
        HostStorage     = Storage | HostVisible | HostCoherent | MapOnCreation,
        DeviceUniform   = TransferDst | Uniform | DeviceLocal,
        DeviceStorage   = TransferDst | Storage | DeviceLocal,
        DeviceIndex     = TransferDst | Index | DeviceLocal,
        DeviceVertex    = TransferDst | Vertex | DeviceLocal,
    };
};
using BufferUsageFlags = flag_t;

/// @brief Buffer class encapsulates a Host or Device buffer and its associated memory.
class R3_API Buffer {
public:
    R3_CTOR_DEFAULT(Buffer);
    R3_COPY_DELETE(Buffer);
    R3_MOVE_DEFAULT(Buffer);

    /// @brief Create a buffer of given size and usage flags.
    /// @param size Size of the buffer in bytes.
    /// @param usage Usage flags for the buffer (see BufferUsage).
    Buffer(usize size, BufferUsageFlags usage);

    /// @brief Destroy the buffer and free its associated memory.
    ~Buffer();

    /// @brief Map the entire buffer memory to CPU accessible address space.
    void map();

    /// @brief Map a range of the buffer memory to CPU accessible address space.
    /// @param offset Offset in bytes from the start of the buffer to map.
    /// @param size Size in bytes of the range to map.
    void mapRange(usize offset, usize size);

    /// @brief Unmap the buffer memory from CPU accessible address space.
    void unmap();

    /// @brief Query if the buffer memory is currently mapped.
    /// @return True if the buffer memory is mapped, false otherwise.
    bool isMapped() const noexcept { return m_mapped != nullptr; }

    /// @brief Copy data from source pointer to the buffer at given offset.
    /// @param src Pointer to the source data to copy from.
    /// @param offset Offset in bytes from the start of the buffer to copy to.
    /// @param size Size in bytes of the data to copy.
    /// @note Buffer must be mapped before calling this function.
    void copy(const void* src, usize offset, usize size);

    /// @brief Flush the entire buffer memory to make host writes visible to the device.
    void flush();

    /// @brief Flush a range of the buffer memory to make host writes visible to the device.
    /// @param offset Offset in bytes from the start of the buffer to flush.
    /// @param size Size in bytes of the range to flush.
    void flushRange(usize offset, usize size);

    /// @brief Invalidate the entire buffer memory to make device writes visible to the host.
    void invalidate();

    /// @brief Invalidate a range of the buffer memory to make device writes visible to the host.
    /// @param offset Offset in bytes from the start of the buffer to invalidate.
    /// @param size Size in bytes of the range to invalidate.
    void invalidateRange(usize offset, usize size);

    /// @brief Query the allocated size of the buffer in bytes.
    /// @return The allocated size of the buffer in bytes, set during construction.
    usize size() const noexcept { return m_size; }

    /// @brief Query the usage flags of the buffer.
    /// @return Usage flags of the buffer, set during construction.
    BufferUsageFlags usage() const noexcept { return m_usage; }

    /// @brief Get the underlying RenderHandle of the buffer as a T&.
    /// @tparam T Type to cast the handle to (e.g., VkBuffer)
    /// @return A reference to the underlying buffer handle.
    template <typename T>
    const T& handle() const noexcept;

private:
    MovableHandle<RenderHandle> m_buffer;
    MovableHandle<RenderHandle> m_memory;
    void* m_mapped           = nullptr;
    usize m_size             = 0;
    BufferUsageFlags m_usage = 0;
};

} // namespace R3