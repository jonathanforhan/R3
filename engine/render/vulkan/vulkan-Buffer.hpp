#pragma once

#include <span>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"

namespace R3 {

class RenderContext;

class Buffer {
public:
    void create(RenderContext& ctx,
                VkDeviceSize size,
                VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties) noexcept(false);

    void destroy() noexcept(true);

    void* map() noexcept(false);

    void unmap() noexcept(true);

    void copyData(const void* data, VkDeviceSize size) noexcept(false);

    template <typename T>
    void copyData(const T& data) noexcept(false) {
        copyData(static_cast<const void*>(&data), sizeof(T));
    }

    template <typename T>
    void copyData(std::span<const T> data) noexcept(false) {
        copyData(static_cast<const void*>(data.data()), data.size_bytes());
    }

    VkBuffer handle() const { return m_buffer; }

    VkDeviceSize size() const { return m_size; }

    bool isValid() const { return m_buffer != VK_NULL_HANDLE; }

private:
    uint32 findMemoryType(VkPhysicalDevice physicalDevice, uint32 typeFilter, VkMemoryPropertyFlags properties) const;

private:
    VkDevice m_device                 = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkBuffer m_buffer                 = VK_NULL_HANDLE;
    VkDeviceMemory m_bufferMemory     = VK_NULL_HANDLE;
    VkDeviceSize m_size               = 0;
    void* m_mappedMemory              = nullptr;
};

} // namespace R3