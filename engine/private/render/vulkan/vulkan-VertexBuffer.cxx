#if R3_VULKAN

#include "render/VertexBuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/CommandPool.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

VertexBuffer::VertexBuffer(const VertexBufferSpecification& spec)
    : m_spec(spec),
      m_vertexCount(static_cast<uint32>(spec.vertices.size())) {
    // staging buffer, CPU writable
    BufferAllocateSpecification stagingAllocateSpecification = {
        .physicalDevice = *m_spec.physicalDevice,
        .logicalDevice = *m_spec.logicalDevice,
        .size = m_spec.vertices.size_bytes(),
        .bufferFlags = uint32(vk::BufferUsageFlagBits::eTransferSrc),
        .memoryFlags = uint32(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent),
    };
    auto [stagingBuffer, stagingMemory] = Buffer::allocate(stagingAllocateSpecification);

    // copy data to staging buffer
    void* data =
        m_spec.logicalDevice->as<vk::Device>().mapMemory(stagingMemory.as<vk::DeviceMemory>(), 0, m_vertexCount, {});
    memcpy(data, m_spec.vertices.data(), m_spec.vertices.size_bytes());
    m_spec.logicalDevice->as<vk::Device>().unmapMemory(stagingMemory.as<vk::DeviceMemory>());

    // real buffer, copy staging buffer to this GPU buffer
    BufferAllocateSpecification bufferAllocateSpecification = {
        .physicalDevice = *m_spec.physicalDevice,
        .logicalDevice = *m_spec.logicalDevice,
        .size = m_spec.vertices.size_bytes(),
        .bufferFlags = uint32(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer),
        .memoryFlags = uint32(vk::MemoryPropertyFlagBits::eDeviceLocal),
    };
    auto [buffer, memory] = Buffer::allocate(bufferAllocateSpecification);

    // copy staging -> buffer
    BufferCopySpecification bufferCopySpecification = {
        .logicalDevice = *m_spec.logicalDevice,
        .commandPool = *m_spec.commandPool,
        .buffer = buffer,
        .stagingBuffer = stagingBuffer,
        .size = m_spec.vertices.size_bytes(),
    };
    Buffer::copy(bufferCopySpecification);

    m_spec.logicalDevice->as<vk::Device>().destroyBuffer(stagingBuffer.as<vk::Buffer>());
    m_spec.logicalDevice->as<vk::Device>().freeMemory(stagingMemory.as<vk::DeviceMemory>());

    setHandle(buffer.handle());
    setDeviceMemory(memory.handle());
}

VertexBuffer::~VertexBuffer() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroyBuffer(as<vk::Buffer>());
        m_spec.logicalDevice->as<vk::Device>().freeMemory(deviceMemoryAs<vk::DeviceMemory>());
    }
}

} // namespace R3

#endif // R3_VULKAN