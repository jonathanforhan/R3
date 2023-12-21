#if R3_VULKAN

#include "render/IndexBuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "render/CommandPool.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

template <std::integral T>
IndexBuffer<T>::IndexBuffer(const IndexBufferSpecification<T>& spec)
    : m_logicalDevice(spec.logicalDevice),
      m_indexCount(static_cast<uint32>(spec.indices.size())) {
    // staging buffer, CPU writable
    const BufferAllocateSpecification stagingAllocateSpecification = {
        .physicalDevice = *spec.physicalDevice,
        .logicalDevice = *spec.logicalDevice,
        .size = spec.indices.size_bytes(),
        .bufferFlags = uint32(vk::BufferUsageFlagBits::eTransferSrc),
        .memoryFlags = uint32(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent),
    };
    auto&& [stagingBuffer, stagingMemory] = Buffer::allocate(stagingAllocateSpecification);

    // copy data to staging buffer
    void* data = spec.logicalDevice->template as<vk::Device>().mapMemory(
        stagingMemory.as<vk::DeviceMemory>(), 0, m_indexCount, {});
    memcpy(data, spec.indices.data(), spec.indices.size_bytes());
    spec.logicalDevice->template as<vk::Device>().unmapMemory(stagingMemory.as<vk::DeviceMemory>());

    // real buffer, copy staging buffer to this GPU buffer
    const BufferAllocateSpecification bufferAllocateSpecification = {
        .physicalDevice = *spec.physicalDevice,
        .logicalDevice = *spec.logicalDevice,
        .size = spec.indices.size_bytes(),
        .bufferFlags = uint32(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer),
        .memoryFlags = uint32(vk::MemoryPropertyFlagBits::eDeviceLocal),
    };
    auto&& [buffer, memory] = Buffer::allocate(bufferAllocateSpecification);

    const BufferCopySpecification bufferCopySpecification = {
        .logicalDevice = *spec.logicalDevice,
        .commandPool = *spec.commandPool,
        .buffer = buffer,
        .stagingBuffer = stagingBuffer,
        .size = spec.indices.size_bytes(),
    };
    Buffer::copy(bufferCopySpecification);

    spec.logicalDevice->template as<vk::Device>().destroyBuffer(stagingBuffer.as<vk::Buffer>());
    spec.logicalDevice->template as<vk::Device>().freeMemory(stagingMemory.as<vk::DeviceMemory>());

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

template class IndexBuffer<uint32>;
template class IndexBuffer<uint16>;

} // namespace R3

#endif // R3_VULKAN
