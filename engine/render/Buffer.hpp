#pragma once

#include <span>
#include <vulkan/vulkan_core.h>
#include "Flags.hpp"
#include "Types.hpp"

namespace R3 {

class RenderContext;

class Buffer {
public:
    void allocate(RenderContext& ctx, usize sizeBytes, BufferUsage usage, MemoryProperties properties);

    void free() noexcept;

    void copy(const void* src, usize size);

    template <typename T>
    void copy(std::span<const T> src) {
        copy(static_cast<const void*>(src.data()), src.size_bytes());
    }

    VkBuffer buffer() noexcept { return m_buffer; }

    usize size() const noexcept { return m_size; }

    bool isValid() const noexcept { return m_buffer != nullptr; }

private:
    void* map();

    void unmap() noexcept;

private:
    VkDevice m_device                 = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkBuffer m_buffer                 = VK_NULL_HANDLE;
    VkDeviceMemory m_bufferMemory     = VK_NULL_HANDLE;
    usize m_size                      = 0;
    void* m_mappedMemory              = nullptr;
};

} // namespace R3