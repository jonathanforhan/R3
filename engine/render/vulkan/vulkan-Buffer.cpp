#if R3_VULKAN

#include "render/Buffer.hpp"

#include <cstring>
#include <format>
#include <vulkan/vulkan_core.h>
#include "Exception.hpp"
#include "Types.hpp"
#include "render/RenderContext.hpp"
#include "vulkan-Check.hpp"

namespace R3 {

static uint32 findMemoryType(VkPhysicalDevice physicalDevice, uint32 typeFilter, MemoryProperties properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32 i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw Exception("Failed to find suitable memory type");
}

void Buffer::create(RenderContext& ctx, usize size, BufferUsage usage, MemoryProperties properties) {
    m_device         = ctx.device();
    m_physicalDevice = ctx.physicalDevice();
    m_size           = size;

    const VkBufferCreateInfo bufferCreateInfo = {
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = {},
        .size                  = size,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
    };
    VK_CHECK(vkCreateBuffer(m_device, &bufferCreateInfo, nullptr, &m_buffer));

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memoryRequirements);

    const VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = memoryRequirements.size,
        .memoryTypeIndex = findMemoryType(m_physicalDevice, memoryRequirements.memoryTypeBits, properties),
    };

    try {
        VK_CHECK(vkAllocateMemory(m_device, &memoryAllocateInfo, nullptr, &m_bufferMemory));
    } catch (const Exception& ex) {
        vkDestroyBuffer(m_device, m_buffer, nullptr);
        throw ex;
    }

    VK_CHECK(vkBindBufferMemory(m_device, m_buffer, m_bufferMemory, 0));
}

void Buffer::destroy() noexcept {
    if (m_mappedMemory) {
        unmap();
    }

    if (m_buffer != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_device, m_buffer, nullptr);
        m_buffer = VK_NULL_HANDLE;
    }

    if (m_bufferMemory != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE) {
        vkFreeMemory(m_device, m_bufferMemory, nullptr);
        m_bufferMemory = VK_NULL_HANDLE;
    }

    m_device         = VK_NULL_HANDLE;
    m_physicalDevice = VK_NULL_HANDLE;
    m_size           = 0;
}

void* Buffer::map() {
    if (m_mappedMemory) {
        throw Exception{__FUNCTION__ " called on already mapped memory"};
    }
    VK_CHECK(vkMapMemory(m_device, m_bufferMemory, 0, m_size, 0, &m_mappedMemory));
    return m_mappedMemory;
}

void Buffer::unmap() noexcept {
    if (m_mappedMemory) {
        vkUnmapMemory(m_device, m_bufferMemory);
        m_mappedMemory = nullptr;
    }
}

void Buffer::copyData(const void* data, usize size) {
    if (size > m_size) {
        throw Exception{std::format("data size {} exceeds buffer size {}", size, m_size)};
    }

    void* mappedData = map();
    std::memcpy(mappedData, data, static_cast<size_t>(size));
    unmap();
}

} // namespace R3

#endif // R3_VULKAN