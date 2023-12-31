#if R3_VULKAN

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/UniformBuffer.hpp"

namespace R3 {

UniformBuffer::UniformBuffer(const UniformBufferSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice),
      m_bufferSize(spec.bufferSize) {
    const BufferAllocateSpecification bufferAllocateSpecification = {
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .size = m_bufferSize,
        .bufferFlags = BufferUsage::UniformBuffer,
        .memoryFlags = MemoryProperty::HostVisible | MemoryProperty::HostCoherent,
    };
    auto&& [buffer, memory] = Buffer::allocate(bufferAllocateSpecification);

    // NOTE we DON'T unmap this memory because mapping isn't free and we write to it every frame
    m_mappedMemory = m_logicalDevice->as<vk::Device>().mapMemory(memory.as<vk::DeviceMemory>(), 0, m_bufferSize, {});

    setHandle(buffer.handle());
    setDeviceMemory(memory.handle());
}

UniformBuffer::~UniformBuffer() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().unmapMemory(deviceMemoryAs<vk::DeviceMemory>());
        m_logicalDevice->as<vk::Device>().destroyBuffer(as<vk::Buffer>());
        m_logicalDevice->as<vk::Device>().freeMemory(deviceMemoryAs<vk::DeviceMemory>());
    }
}

void UniformBuffer::write(const void* data, usize size, usize offset) {
    CHECK(data != nullptr);
    CHECK(size + offset <= m_bufferSize);
    memcpy(reinterpret_cast<uint8*>(m_mappedMemory) + offset, data, size);
}

} // namespace R3

#endif // R3_VULKAN