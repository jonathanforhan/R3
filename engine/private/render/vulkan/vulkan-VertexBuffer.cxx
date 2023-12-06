#if R3_VULKAN

#include "render/VertexBuffer.hpp"

#include <vulkan/vulkan.hpp>
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

    vk::BufferCreateInfo bufferCreateInfo = {
        .sType = vk::StructureType::eBufferCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .size = spec.vertices.size_bytes(),
        .usage = vk::BufferUsageFlagBits::eVertexBuffer,
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = indices,
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createBuffer(bufferCreateInfo));

    auto memoryRequirements = m_spec.logicalDevice->as<vk::Device>().getBufferMemoryRequirements(as<vk::Buffer>());

    vk::MemoryAllocateInfo memoryAllocateInfo = {
        .sType = vk::StructureType::eMemoryAllocateInfo,
        .pNext = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = m_spec.physicalDevice->queryMemoryType(
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
    };

    setBuffer(m_spec.logicalDevice->as<vk::Device>().allocateMemory(memoryAllocateInfo));
    m_spec.logicalDevice->as<vk::Device>().bindBufferMemory(as<vk::Buffer>(), bufferAs<vk::DeviceMemory>(), 0);

    void* data = m_spec.logicalDevice->as<vk::Device>().mapMemory(
        bufferAs<vk::DeviceMemory>(), 0, m_spec.vertices.size_bytes(), {});

    memcpy(data, m_spec.vertices.data(), m_spec.vertices.size_bytes());

    m_spec.logicalDevice->as<vk::Device>().unmapMemory(bufferAs<vk::DeviceMemory>());
}

void VertexBuffer::destroy() {
    m_spec.logicalDevice->as<vk::Device>().destroyBuffer(as<vk::Buffer>());
    m_spec.logicalDevice->as<vk::Device>().freeMemory(bufferAs<vk::DeviceMemory>());
}

} // namespace R3

#endif // R3_VULKAN