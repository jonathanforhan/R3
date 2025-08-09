#include "vulkan-Buffer.hpp"

#include <cstring>
#include <format>
#include <vulkan/vulkan_core.h>
#include <Exception.hpp>
#include <Types.hpp>
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3 {

void Buffer::create(RenderContext& ctx,
                    VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties) noexcept(false) {
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

void Buffer::destroy() noexcept(true) {
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

void* Buffer::map() noexcept(false) {
    if (m_mappedMemory) {
        return m_mappedMemory;
    }
    VK_CHECK(vkMapMemory(m_device, m_bufferMemory, 0, m_size, 0, &m_mappedMemory));
    return m_mappedMemory;
}

void Buffer::unmap() noexcept(true) {
    if (m_mappedMemory) {
        vkUnmapMemory(m_device, m_bufferMemory);
        m_mappedMemory = nullptr;
    }
}

void Buffer::copyData(const void* data, VkDeviceSize size) noexcept(false) {
    if (size > m_size) {
        throw Exception{std::format("data size {} exceeds buffer size {}", size, m_size)};
    }

    void* mappedData = map();
    std::memcpy(mappedData, data, static_cast<size_t>(size));
    unmap();
}

uint32 Buffer::findMemoryType(VkPhysicalDevice physicalDevice,
                              uint32 typeFilter,
                              VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32 i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw Exception("Failed to find suitable memory type");
}

} // namespace R3