#if R3_VULKAN

#include "vulkan-VertexBuffer.hxx"

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <cstring>
#include <expected>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<VertexBuffer> VertexBuffer::create(const VertexBufferSpecification& spec) {
    VertexBuffer self;
    self.m_device      = spec.device.vk();
    self.m_vertexCount = static_cast<uint32>(spec.vertices.size());

    // must first create host visible staging buffer to copy data
    // then create a device local buffer, copy the staging to that and it will be faster

    const uint32 indices[] = {
        spec.device.graphicsQueue().index(),
        spec.device.presentationQueue().index(),
    };

    const VkBufferCreateInfo stagingBufferCreateInfo = {
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = {},
        .size                  = static_cast<uint32>(spec.vertices.size_bytes()),
        .usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = static_cast<uint32>(std::size(indices)),
        .pQueueFamilyIndices   = indices,
    };

    VkBuffer stagingBuffer;
    if (VkResult result = vkCreateBuffer(self.m_device, &stagingBufferCreateInfo, nullptr, &stagingBuffer)) {
        R3_LOG(Error, "vkCreateBuffer failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }
    self.VulkanObject<VkBuffer>::m_handle = stagingBuffer; // NOTE set temporarily incase of early exit

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(self.m_device, stagingBuffer, &memoryRequirements);

    const VkMemoryPropertyFlags memoryFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    const VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = memoryRequirements.size,
        .memoryTypeIndex = spec.physicalDevice.queryMemoryType(memoryRequirements.memoryTypeBits, memoryFlags),
    };

    VkDeviceMemory stagingMemory;
    if (VkResult result = vkAllocateMemory(self.m_device, &memoryAllocateInfo, nullptr, &stagingMemory)) {
        R3_LOG(Error, "vkAllocateMemory failure {}", (int)result);
        return std::unexpected(Error::AllocationFailure);
    }
    self.VulkanObject<VkDeviceMemory>::m_handle = stagingMemory; // NOTE set temporarily incase of early exit

    void* data;
    if (VkResult result = vkMapMemory(self.m_device, stagingMemory, 0, self.m_vertexCount, {}, &data)) {
        R3_LOG(Error, "vkMapMemory failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    // copy vertex data to mapped memory
    memcpy(data, spec.vertices.data(), spec.vertices.size_bytes());
    vkUnmapMemory(self.m_device, stagingMemory);

    return self;
}

VertexBuffer::~VertexBuffer() {
    vkDestroyBuffer(m_device, VulkanObject<VkBuffer>::vk(), nullptr);
    vkFreeMemory(m_device, VulkanObject<VkDeviceMemory>::vk(), nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN
