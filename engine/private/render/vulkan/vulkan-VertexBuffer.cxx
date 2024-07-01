#if R3_VULKAN

#include "vulkan-VertexBuffer.hxx"

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <expected>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<VertexBuffer> VertexBuffer::create(const VertexBufferSpecification& spec) {
    VertexBuffer self;
    self.m_device      = spec.device.vk();
    self.m_vertexCount = static_cast<uint32>(spec.vertices.size());

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
    VkResult result = vkCreateBuffer(self.m_device, &stagingBufferCreateInfo, nullptr, &stagingBuffer);
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "vkCreateBuffer failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(self.m_device, stagingBuffer, &memoryRequirements);

    // basically have to count on this being successful, if not the program in uncoverable
    uint32 memoryTypeIndex = *spec.physicalDevice.queryMemoryType(
        memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    const VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };

    VkDeviceMemory stagingMemory;
    result = vkAllocateMemory(self.m_device, &memoryAllocateInfo, nullptr, &stagingMemory);
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "vkAllocateMemory failure {}", (int)result);
        return std::unexpected(Error::AllocationFailure);
    }

    void* data;
    result = vkMapMemory(self.m_device, stagingMemory, 0, self.m_vertexCount, {}, &data);
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "vkMapMemory failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    return self;
}

VertexBuffer::~VertexBuffer() {}

} // namespace R3::vulkan

#endif // R3_VULKAN
