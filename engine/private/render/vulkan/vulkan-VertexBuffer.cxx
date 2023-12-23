#if R3_VULKAN

#include "render/VertexBuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/CommandPool.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

VertexBuffer::VertexBuffer(const VertexBufferSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice),
      m_vertexCount(static_cast<uint32>(spec.vertices.size())) {
    // staging buffer, CPU writable
    const BufferAllocateSpecification stagingAllocateSpecification = {
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .size = spec.vertices.size_bytes(),
        .bufferFlags = BufferUsage::TransferSrc,
        .memoryFlags = MemoryProperty::HostVisible | MemoryProperty::HostCoherent,
    };
    auto&& [stagingBuffer, stagingMemory] = Buffer::allocate(stagingAllocateSpecification);

    // copy data to staging buffer
    void* data =
        m_logicalDevice->as<vk::Device>().mapMemory(stagingMemory.as<vk::DeviceMemory>(), 0, m_vertexCount, {});
    memcpy(data, spec.vertices.data(), spec.vertices.size_bytes());
    m_logicalDevice->as<vk::Device>().unmapMemory(stagingMemory.as<vk::DeviceMemory>());

    // real buffer, copy staging buffer to this GPU buffer
    const BufferAllocateSpecification bufferAllocateSpecification = {
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .size = spec.vertices.size_bytes(),
        .bufferFlags = BufferUsage::TransferDst | BufferUsage::VertexBuffer,
        .memoryFlags = MemoryProperty::DeviceLocal,
    };
    auto&& [buffer, memory] = Buffer::allocate(bufferAllocateSpecification);

    // copy staging -> buffer
    const BufferCopySpecification bufferCopySpecification = {
        .logicalDevice = *m_logicalDevice,
        .commandPool = spec.commandPool,
        .buffer = buffer,
        .stagingBuffer = stagingBuffer,
        .size = spec.vertices.size_bytes(),
    };
    Buffer::copy(bufferCopySpecification);

    m_logicalDevice->as<vk::Device>().destroyBuffer(stagingBuffer.as<vk::Buffer>());
    m_logicalDevice->as<vk::Device>().freeMemory(stagingMemory.as<vk::DeviceMemory>());

    setHandle(buffer.handle());
    setDeviceMemory(memory.handle());
}

VertexBuffer::~VertexBuffer() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyBuffer(as<vk::Buffer>());
        m_logicalDevice->as<vk::Device>().freeMemory(deviceMemoryAs<vk::DeviceMemory>());
    }
}

} // namespace R3

#endif // R3_VULKAN