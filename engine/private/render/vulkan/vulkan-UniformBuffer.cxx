#if R3_VULKAN

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/RenderSpecification.hpp"
#include "render/UniformBuffer.hpp"

namespace R3 {

UniformBuffer::UniformBuffer(const UniformBufferSpecification& spec)
    : m_spec(spec) {
    const BufferAllocateSpecification bufferAllocateSpecification = {
        .physicalDevice = *m_spec.physicalDevice,
        .logicalDevice = *m_spec.logicalDevice,
        .size = m_spec.bufferSize,
        .bufferFlags = uint32(vk::BufferUsageFlagBits::eUniformBuffer),
        .memoryFlags = uint32(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent),
    };
    auto [buffer, memory] = Buffer::allocate(bufferAllocateSpecification);

    // NOTE we DON'T unmap this memory because mapping isn't free and we write to it every frame
    m_mappedMemory =
        m_spec.logicalDevice->as<vk::Device>().mapMemory(memory.as<vk::DeviceMemory>(), 0, m_spec.bufferSize, {});

    setHandle(buffer.handle());
    setDeviceMemory(memory.handle());
}

UniformBuffer::~UniformBuffer() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().unmapMemory(deviceMemoryAs<vk::DeviceMemory>());
        m_spec.logicalDevice->as<vk::Device>().destroyBuffer(as<vk::Buffer>());
        m_spec.logicalDevice->as<vk::Device>().freeMemory(deviceMemoryAs<vk::DeviceMemory>());
    }
}

void UniformBuffer::update(void* buffer, usize size, usize offset) {
    CHECK(buffer != nullptr);
    CHECK(size + offset <= m_spec.bufferSize);
    memcpy(reinterpret_cast<uint8*>(m_mappedMemory) + offset, buffer, size);
}

} // namespace R3

#endif // R3_VULKAN