#if R3_VULKAN

#include "render/IndexBuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/CommandPool.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

template <std::integral T>
IndexBuffer<T>::IndexBuffer(const IndexBufferSpecification<T>& spec)
    : m_spec(spec),
      m_indexCount(static_cast<uint32>(spec.indices.size())) {

    // staging buffer, CPU writable
    BufferAllocateSpecification stagingAllocateSpecification = {
        .physicalDevice = *m_spec.physicalDevice,
        .logicalDevice = *m_spec.logicalDevice,
        .size = m_spec.indices.size_bytes(),
        .bufferFlags = uint32(vk::BufferUsageFlagBits::eTransferSrc),
        .memoryFlags = uint32(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent),
    };
    auto [stagingBuffer, stagingMemory] = Buffer::allocate(stagingAllocateSpecification);

    // copy data to staging buffer
    void* data = m_spec.logicalDevice->as<vk::Device>().mapMemory((VkDeviceMemory)stagingMemory, 0, m_indexCount, {});
    memcpy(data, m_spec.indices.data(), m_spec.indices.size_bytes());
    m_spec.logicalDevice->as<vk::Device>().unmapMemory((VkDeviceMemory)stagingMemory);

    // real buffer, copy staging buffer to this GPU buffer
    BufferAllocateSpecification bufferAllocateSpecification = {
        .physicalDevice = *m_spec.physicalDevice,
        .logicalDevice = *m_spec.logicalDevice,
        .size = m_spec.indices.size_bytes(),
        .bufferFlags = uint32(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer),
        .memoryFlags = uint32(vk::MemoryPropertyFlagBits::eDeviceLocal),
    };
    auto [buffer, memory] = Buffer::allocate(bufferAllocateSpecification);

    BufferCopySpecification bufferCopySpecification = {
        .logicalDevice = *m_spec.logicalDevice,
        .commandPool = *m_spec.commandPool,
        .buffer = buffer,
        .stagingBuffer = stagingBuffer,
        .size = m_spec.indices.size_bytes(),
    };
    Buffer::copy(bufferCopySpecification);

    m_spec.logicalDevice->as<vk::Device>().destroyBuffer((VkBuffer)stagingBuffer);
    m_spec.logicalDevice->as<vk::Device>().freeMemory((VkDeviceMemory)stagingMemory);

    setHandle(buffer);
    setDeviceMemory(memory);
}

template <std::integral T>
IndexBuffer<T>::~IndexBuffer() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroyBuffer(as<vk::Buffer>());
        m_spec.logicalDevice->as<vk::Device>().freeMemory(deviceMemoryAs<vk::DeviceMemory>());
    }
}

template class IndexBuffer<uint32>;
template class IndexBuffer<uint16>;

} // namespace R3

#endif // R3_VULKAN
