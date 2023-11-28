#if R3_VULKAN

#include "render/VertexBuffer.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

void VertexBuffer::create(const VertexBufferSpecification& spec) {
    CHECK(spec.physicalDevice != nullptr);
    CHECK(spec.logicalDevice != nullptr);
    m_spec = spec;

    uint32 indices[]{
        m_spec.logicalDevice->graphicsQueue().index(),
        m_spec.logicalDevice->presentationQueue().index(),
    };
    VkBufferCreateInfo bufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .size = spec.vertices.size_bytes(),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = indices,
    };

    ENSURE(
        vkCreateBuffer(m_spec.logicalDevice->handle<VkDevice>(), &bufferCreateInfo, nullptr, handlePtr<VkBuffer*>()) ==
        VK_SUCCESS);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_spec.logicalDevice->handle<VkDevice>(), handle<VkBuffer>(), &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = m_spec.physicalDevice->queryMemoryType(
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
    };

    ENSURE(vkAllocateMemory(
               m_spec.logicalDevice->handle<VkDevice>(), &memoryAllocateInfo, nullptr, bufferPtr<VkDeviceMemory*>()) ==
           VK_SUCCESS);
    vkBindBufferMemory(m_spec.logicalDevice->handle<VkDevice>(), handle<VkBuffer>(), buffer<VkDeviceMemory>(), 0);

    void* data;
    ENSURE(vkMapMemory(m_spec.logicalDevice->handle<VkDevice>(),
                       buffer<VkDeviceMemory>(),
                       0,
                       m_spec.vertices.size_bytes(),
                       0,
                       &data) == VK_SUCCESS);
    memcpy(data, m_spec.vertices.data(), m_spec.vertices.size_bytes());
    vkUnmapMemory(m_spec.logicalDevice->handle<VkDevice>(), buffer<VkDeviceMemory>());
}

void VertexBuffer::destroy() {
    vkDestroyBuffer(m_spec.logicalDevice->handle<VkDevice>(), handle<VkBuffer>(), nullptr);
    vkFreeMemory(m_spec.logicalDevice->handle<VkDevice>(), buffer<VkDeviceMemory>(), nullptr);
}

} // namespace R3

#endif // R3_VULKAN