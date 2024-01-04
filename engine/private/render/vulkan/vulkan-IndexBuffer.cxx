#if R3_VULKAN

#include "render/IndexBuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "render/CommandPool.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

template <std::integral T>
IndexBuffer<T>::IndexBuffer(const IndexBufferSpecification<T>& spec)
    : m_logicalDevice(&spec.logicalDevice),
      m_indexCount(static_cast<uint32>(spec.indices.size())) {
    // staging buffer, CPU writable
    const BufferAllocateSpecification stagingAllocateSpecification = {
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .size = spec.indices.size_bytes(),
        .bufferFlags = BufferUsage::TransferSrc,
        .memoryFlags = MemoryProperty::HostVisible | MemoryProperty::HostCoherent,
    };
    auto&& [stagingBuffer, stagingMemory] = Buffer::allocate(stagingAllocateSpecification);

    // copy data to staging buffer
    void* data =
        m_logicalDevice->template as<vk::Device>().mapMemory(stagingMemory.as<vk::DeviceMemory>(), 0, m_indexCount, {});
    memcpy(data, spec.indices.data(), spec.indices.size_bytes());
    m_logicalDevice->template as<vk::Device>().unmapMemory(stagingMemory.as<vk::DeviceMemory>());

    // real buffer, copy staging buffer to this GPU buffer
    const BufferAllocateSpecification bufferAllocateSpecification = {
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .size = spec.indices.size_bytes(),
        .bufferFlags = BufferUsage::TransferDst | BufferUsage::IndexBuffer,
        .memoryFlags = MemoryProperty::DeviceLocal,
    };
    auto&& [buffer, memory] = Buffer::allocate(bufferAllocateSpecification);

    const BufferCopySpecification bufferCopySpecification = {
        .logicalDevice = *m_logicalDevice,
        .commandBuffer = spec.commandBuffer,
        .buffer = buffer,
        .stagingBuffer = stagingBuffer,
        .size = spec.indices.size_bytes(),
    };
    Buffer::copy(bufferCopySpecification);

    m_logicalDevice->template as<vk::Device>().destroyBuffer(stagingBuffer.as<vk::Buffer>());
    m_logicalDevice->template as<vk::Device>().freeMemory(stagingMemory.as<vk::DeviceMemory>());

    setHandle(buffer.handle());
    setDeviceMemory(memory.handle());
}

template <std::integral T>
IndexBuffer<T>::~IndexBuffer() {
    if (validHandle()) {
        m_logicalDevice->template as<vk::Device>().destroyBuffer(as<vk::Buffer>());
        m_logicalDevice->template as<vk::Device>().freeMemory(deviceMemoryAs<vk::DeviceMemory>());
    }
}

template class R3_API IndexBuffer<uint32>;
template class R3_API IndexBuffer<uint16>;

} // namespace R3

#endif // R3_VULKAN
