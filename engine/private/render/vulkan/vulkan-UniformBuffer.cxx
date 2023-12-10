#if R3_VULKAN

#include <vulkan/vulkan.hpp>
#include "render/UniformBuffer.hpp"
#include "render/RenderSpecification.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "api/Check.hpp"

namespace R3 {

UniformBuffer::UniformBuffer(const UniformBufferSpecification& spec)
    : m_spec(spec) {

    BufferAllocateSpecification bufferAllocateSpecification = {
        .physicalDevice = *m_spec.physicalDevice,
        .logicalDevice = *m_spec.logicalDevice,
        .size = m_spec.bufferSize,
        .bufferFlags = uint32(vk::BufferUsageFlagBits::eUniformBuffer),
        .memoryFlags = uint32(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent),
    };
    auto [buffer, memory] = Buffer::allocate(bufferAllocateSpecification);

    // NOTE we DON'T unmap this memory because mapping isn't free and we write to it every frame
    m_mappedMemory = m_spec.logicalDevice->as<vk::Device>().mapMemory((VkDeviceMemory)memory, 0, m_spec.bufferSize, {});
    setHandle(buffer);
    setDeviceMemory(memory);
}

UniformBuffer::~UniformBuffer() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().unmapMemory(deviceMemoryAs<vk::DeviceMemory>());
        m_spec.logicalDevice->as<vk::Device>().destroyBuffer(as<vk::Buffer>());
        m_spec.logicalDevice->as<vk::Device>().freeMemory(deviceMemoryAs<vk::DeviceMemory>());
    }
}

void UniformBuffer::update(void* buffer, usize size) {
    CHECK(buffer != nullptr);
    CHECK(size <= m_spec.bufferSize);
    memcpy(m_mappedMemory, buffer, size);
}

} // namespace R3

#endif // R3_VULKAN