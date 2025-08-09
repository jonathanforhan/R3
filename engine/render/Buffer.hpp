#pragma once

#include <span>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"

namespace R3 {

struct MemoryPropertyFlags {
    enum : uint32 {
        DeviceLocal       = 0x00000001,
        HostVisible       = 0x00000002,
        HostCoherent      = 0x00000004,
        HostCached        = 0x00000008,
        LazilyAllocated   = 0x00000010,
        Protected         = 0x00000020,
        DeviceCoherentAmd = 0x00000040,
        DeviceUncachedAmd = 0x00000080,
        RdmaCapableNv     = 0x00000100,
    };
};
using MemoryProperties = uint32;

struct BufferUsageFlags {
    enum : uint32 {
        TransferSrc        = 0x00000001,
        TransferDst        = 0x00000002,
        UniformTexelBuffer = 0x00000004,
        StorageTexelBuffer = 0x00000008,
        UniformBuffer      = 0x00000010,
        StorageBuffer      = 0x00000020,
        IndexBuffer        = 0x00000040,
        VertexBuffer       = 0x00000080,
        IndirectBuffer     = 0x00000100,
    };
};
using BufferUsage = uint32;

class RenderContext;

class Buffer {
public:
    void create(RenderContext& ctx, usize size, BufferUsage usage, MemoryProperties properties);

    void destroy() noexcept;

    void* map();

    void unmap() noexcept;

    void copyData(const void* data, usize size);

    void copyData(const auto& data) { copyData(static_cast<const void*>(&data), sizeof(data)); }

    template <typename T>
    void copyData(std::span<const T> data) {
        copyData(static_cast<const void*>(data.data()), data.size_bytes());
    }

    VkBuffer handle() noexcept { return m_buffer; }

    usize size() const noexcept { return m_size; }

    bool isValid() const noexcept { return m_buffer != nullptr; }

private:
    VkDevice m_device                 = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkBuffer m_buffer                 = VK_NULL_HANDLE;
    VkDeviceMemory m_bufferMemory     = VK_NULL_HANDLE;
    usize m_size                      = 0;
    void* m_mappedMemory              = nullptr;
};

} // namespace R3