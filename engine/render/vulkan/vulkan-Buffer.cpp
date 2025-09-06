#include "engine/render/Buffer.hpp"

#include <cstring>
#include <vulkan/vulkan.h>
#include "api/Assert.hpp"
#include "api/MovableHandle.hpp"
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"

#define TO_VK_USAGE_FLAGS(usage)  ((VkBufferUsageFlags)((usage) & 0x0000'FFFF))
#define TO_VK_MEMORY_FLAGS(usage) (((VkMemoryPropertyFlags)((usage) >> 16)) & 0xFF)

extern VkDevice g_device;

namespace R3 {

Buffer::Buffer(usize size, BufferUsageFlags usage)
    : m_size{size},
      m_usage{usage} {
    R3_ASSERT(usage & (BufferUsage::HostVisible | BufferUsage::DeviceLocal),
              "Buffer usage must include either HostVisible or DeviceLocal");

    const VkBufferUsageFlags vkUsageFlags  = TO_VK_USAGE_FLAGS(usage);
    const VkMemoryPropertyFlags vkMemFlags = TO_VK_MEMORY_FLAGS(usage);

    try {
        const VkBufferCreateInfo bufferInfo = {
            .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size                  = size,
            .usage                 = vkUsageFlags,
            .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices   = nullptr, /* only needed when sharingMode == VK_SHARING_MODE_CONCURRENT */
        };
        VK_CHECK(vkCreateBuffer(g_device, &bufferInfo, nullptr, &m_buffer.get<VkBuffer>()));

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(g_device, m_buffer.get<VkBuffer>(), &memoryRequirements);

        const VkMemoryAllocateInfo memoryInfo = {
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext           = nullptr,
            .allocationSize  = memoryRequirements.size,
            .memoryTypeIndex = GEngine()->RenderContext<vulkan::RenderContext>().queryDeviceMemoryTypeIndex(
                memoryRequirements.memoryTypeBits, vkMemFlags),
        };
        VK_CHECK(vkAllocateMemory(g_device, &memoryInfo, nullptr, &m_memory.get<VkDeviceMemory>()));
        VK_CHECK(vkBindBufferMemory(g_device, m_buffer.get<VkBuffer>(), m_memory.get<VkDeviceMemory>(), 0));

        if (usage & BufferUsage::MapOnCreation) {
            map();
        }
    } catch (...) {
        this->~Buffer();
        throw;
    }
}

Buffer::~Buffer() {
    vkDestroyBuffer(g_device, m_buffer.get<VkBuffer>(), nullptr);
    if (m_memory && m_mapped) {
        vkUnmapMemory(g_device, m_memory.get<VkDeviceMemory>());
    }
    vkFreeMemory(g_device, m_memory.get<VkDeviceMemory>(), nullptr);
}

void Buffer::map() {
    R3_ASSERT(TO_VK_MEMORY_FLAGS(m_usage) & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    VK_CHECK(vkMapMemory(g_device, m_memory.get<VkDeviceMemory>(), 0, m_size, 0, &m_mapped));
}

void Buffer::mapRange(usize offset, usize size) {
    VK_CHECK(vkMapMemory(g_device, m_memory.get<VkDeviceMemory>(), offset, size, 0, &m_mapped));
}

void Buffer::unmap() {
    vkUnmapMemory(g_device, m_memory.get<VkDeviceMemory>());
    m_mapped = nullptr;
}

void Buffer::copy(const void* src, usize offset, usize size) {
    R3_ASSERT(m_mapped, "memory must be mapped prior to copy");
    R3_ASSERT(offset + size <= m_size, "range exceeds buffer size");
    (void)std::memcpy((uint8*)m_mapped + offset, src, size);
}

void Buffer::flush() {
    const VkMappedMemoryRange memoryRange = {
        .sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .memory = m_memory.get<VkDeviceMemory>(),
        .offset = 0,
        .size   = m_size,
    };
    VK_CHECK(vkFlushMappedMemoryRanges(g_device, 1, &memoryRange));
}

void Buffer::flushRange(usize offset, usize size) {
    R3_ASSERT(offset + size <= m_size, "range exceeds buffer size");
    const VkMappedMemoryRange memoryRange = {
        .sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .memory = m_memory.get<VkDeviceMemory>(),
        .offset = offset,
        .size   = size,
    };
    VK_CHECK(vkFlushMappedMemoryRanges(g_device, 1, &memoryRange));
}

void Buffer::invalidate() {
    const VkMappedMemoryRange memoryRange = {
        .sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .memory = m_memory.get<VkDeviceMemory>(),
        .offset = 0,
        .size   = m_size,
    };
    VK_CHECK(vkInvalidateMappedMemoryRanges(g_device, 1, &memoryRange));
}

void Buffer::invalidateRange(usize offset, usize size) {
    R3_ASSERT(offset + size <= m_size, "range exceeds buffer size");
    const VkMappedMemoryRange memoryRange = {
        .sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .memory = m_memory.get<VkDeviceMemory>(),
        .offset = offset,
        .size   = size,
    };
    VK_CHECK(vkInvalidateMappedMemoryRanges(g_device, 1, &memoryRange));
}

} // namespace R3
