#pragma once

#include <span>
#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "api/Types.hpp"
#include "vulkan-Fwd.hpp"
#include "vulkan-Handle.hpp"

namespace R3::vulkan {

class Buffer {
public:
    R3_CTOR_DEFAULT(Buffer);
    R3_COPY_DELETE(Buffer);
    R3_MOVE_DEFAULT(Buffer);

    Buffer(RenderContext& ctx, usize sizeBytes, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    ~Buffer() noexcept;

    void copy(const void* src, usize size);

    template <typename T>
    void copy(std::span<const T> src) {
        copy(static_cast<const void*>(src.data()), src.size_bytes());
    }

    VkBuffer buffer() const noexcept { return m_buffer; }

    usize size() const noexcept { return m_size; }

private:
    void* map();

    void unmap() noexcept;

private:
    Handle<VkDevice> m_device;
    Handle<VkBuffer> m_buffer;
    Handle<VkDeviceMemory> m_bufferMemory;
    usize m_size         = 0;
    void* m_mappedMemory = nullptr;
};

} // namespace R3::vulkan