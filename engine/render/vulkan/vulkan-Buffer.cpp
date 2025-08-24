#include "vulkan-Buffer.hpp"

#include <cstring>
#include <format>
#include <vulkan/vulkan_core.h>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

Buffer::Buffer(RenderContext& ctx, usize sizeBytes, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    : m_device(ctx.device()),
      m_size(sizeBytes) {
    const VkBufferCreateInfo bufferInfo = {
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = {},
        .size                  = sizeBytes,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr, /* only needed when sharingMode == VK_SHARING_MODE_CONCURRENT */
    };
    VK_CHECK(vkCreateBuffer(m_device, &bufferInfo, nullptr, &*m_buffer));

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memoryRequirements);

    const VkMemoryAllocateInfo memoryInfo = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = memoryRequirements.size,
        .memoryTypeIndex = ctx.queryDeviceMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties),
    };

    try {
        VK_CHECK(vkAllocateMemory(m_device, &memoryInfo, nullptr, &*m_bufferMemory));
    } catch (const Exception& ex) {
        vkDestroyBuffer(m_device, m_buffer, nullptr);
        throw ex;
    }

    VK_CHECK(vkBindBufferMemory(m_device, m_buffer, m_bufferMemory, 0));
}

Buffer::~Buffer() noexcept {
    if (m_mappedMemory) {
        unmap();
    }

    if (m_device) {
        vkDestroyBuffer(m_device, m_buffer, nullptr);
        vkFreeMemory(m_device, m_bufferMemory, nullptr);
    }
}

void Buffer::copy(const void* src, usize size) {
    if (size > m_size) {
        throw Exception{std::format("data size {} exceeds buffer size {}", size, m_size)};
    }

    void* mappedData = map();
    std::memcpy(mappedData, src, static_cast<size_t>(size));
    unmap();
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

} // namespace R3::vulkan
