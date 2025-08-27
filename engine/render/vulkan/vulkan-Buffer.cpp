#include "vulkan-Buffer.hpp"

#include <cstring>
#include <vulkan/vulkan_core.h>
#include "api/Assert.hpp"
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "render/Flags.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

Buffer::~Buffer() noexcept {
    RenderContext& ctx = static_cast<RenderContext&>(Engine()->context());
    vkDestroyBuffer(ctx.device(), m_buffer, nullptr);
    if (m_bufferMemory && m_mapped) {
        vkUnmapMemory(ctx.device(), m_bufferMemory);
    }
    vkFreeMemory(ctx.device(), m_bufferMemory, nullptr);
}

void Buffer::copy(const void* src, usize sizeBytes, usize offset) {
    RenderContext& ctx = static_cast<RenderContext&>(Engine()->context());
    if (!m_mapped) {
        VK_CHECK(vkMapMemory(ctx.device(), m_bufferMemory, 0, sizeBytes, 0, &m_mapped));
    }
    std::memcpy((uint8*)m_mapped + offset, src, sizeBytes);
}

void Buffer::create(const void* src, usize sizeBytes, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    RenderContext& ctx = static_cast<RenderContext&>(Engine()->context());

    try {
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
        VK_CHECK(vkCreateBuffer(ctx.device(), &bufferInfo, nullptr, &*m_buffer));

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(ctx.device(), m_buffer, &memoryRequirements);

        const VkMemoryAllocateInfo memoryInfo = {
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext           = nullptr,
            .allocationSize  = memoryRequirements.size,
            .memoryTypeIndex = ctx.queryDeviceMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties),
        };
        VK_CHECK(vkAllocateMemory(ctx.device(), &memoryInfo, nullptr, &*m_bufferMemory));
        VK_CHECK(vkBindBufferMemory(ctx.device(), m_buffer, m_bufferMemory, 0));

        /* if user data */
        if (src) {
            R3_ASSERT(properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT && "must be host visible to write directly");
            copy(src, sizeBytes);
        }
    } catch (const Exception& ex) {
        vkFreeMemory(ctx.device(), m_bufferMemory, nullptr);
        vkDestroyBuffer(ctx.device(), m_buffer, nullptr);
        throw ex;
    }
}

void Buffer::create(const void* src, usize sizeBytes, BufferPreset preset) {
    switch (preset) {
        case BufferPreset::Staging:
            create(src,
                   sizeBytes,
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            break;
        case BufferPreset::HostUniform:
            create(src,
                   sizeBytes,
                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            break;
        case BufferPreset::DeviceVertex:
            create(src,
                   sizeBytes,
                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            break;
        case BufferPreset::DeviceIndex:
            create(src,
                   sizeBytes,
                   VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            break;
        default:
            throw Exception{"Unknown BufferPreset"};
    }
}

} // namespace R3::vulkan
