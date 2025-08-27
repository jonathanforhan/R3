#pragma once

#include <span>
#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "api/Types.hpp"
#include "render/Flags.hpp"
#include "vulkan-Handle.hpp"

namespace R3::vulkan {

/// @brief RAII Buffer abstraction for Vulkan buffers
class Buffer {
public:
    R3_CTOR_DEFAULT(Buffer);
    R3_COPY_DELETE(Buffer);
    R3_MOVE_DEFAULT(Buffer);

    /// @brief Allocate a Vulkan Buffer
    /// @param src Data to copy into the buffer, can be nullptr to allocate uninitialized
    /// @param sizeBytes Size of the buffer in bytes
    /// @param usage Usage flags passed to vkCreateBuffer
    /// @param properties Memory properties passed to vkAllocateMemory
    Buffer(const void* src, usize sizeBytes, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
        create(src, sizeBytes, usage, properties);
    }

    template <typename T>
    Buffer(std::span<const T> src, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
        create(static_cast<const void*>(src.data()), src.size_bytes(), usage, properties);
    }

    Buffer(const void* src, usize sizeBytes, BufferPreset preset) { create(src, sizeBytes, preset); }

    template <typename T>
    Buffer(std::span<const T> src, BufferPreset preset) {
        create(static_cast<const void*>(src.data()), src.size_bytes(), preset);
    }

    ~Buffer() noexcept;

    void copy(const void* src, usize sizeBytes, usize offset = 0);

    VkBuffer buffer() const noexcept { return m_buffer; }

private:
    void create(const void* src, usize sizeBytes, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    void create(const void* src, usize sizeBytes, BufferPreset preset);

private:
    Handle<VkBuffer> m_buffer;
    Handle<VkDeviceMemory> m_bufferMemory;
    void* m_mapped = nullptr;
};

} // namespace R3::vulkan