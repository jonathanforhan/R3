#if R3_VULKAN

#include "render/VertexBuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/CommandPool.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

VertexBuffer::VertexBuffer(const VertexBufferSpecification& spec)
    : m_spec(spec) {
    m_vertexCount = static_cast<uint32>(spec.vertices.size());

    // staging buffer, CPU writable
    auto [stagingBuffer, stagingMemory] =
        Buffer::allocate(*m_spec.logicalDevice,
                         *m_spec.physicalDevice,
                         m_spec.vertices.size_bytes(),
                         uint32(vk::BufferUsageFlagBits::eTransferSrc),
                         uint32(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

    // copy data to staging buffer
    void* data = m_spec.logicalDevice->as<vk::Device>().mapMemory((VkDeviceMemory)stagingMemory, 0, m_vertexCount, {});

    memcpy(data, m_spec.vertices.data(), m_spec.vertices.size_bytes());
    m_spec.logicalDevice->as<vk::Device>().unmapMemory((VkDeviceMemory)stagingMemory);

    // real buffer, copy staging buffer to this GPU buffer
    auto [buffer, memory] =
        Buffer::allocate(*m_spec.logicalDevice,
                         *m_spec.physicalDevice,
                         m_spec.vertices.size_bytes(),
                         uint32(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer),
                         uint32(vk::MemoryPropertyFlagBits::eDeviceLocal));

    Buffer::copy(buffer, stagingBuffer, m_spec.vertices.size_bytes(), *m_spec.logicalDevice, *m_spec.commandPool);
    m_spec.logicalDevice->as<vk::Device>().destroyBuffer((VkBuffer)stagingBuffer);
    m_spec.logicalDevice->as<vk::Device>().freeMemory((VkDeviceMemory)stagingMemory);

    setHandle(buffer);
    setDeviceMemory(memory);
}

VertexBuffer::~VertexBuffer() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroyBuffer(as<vk::Buffer>());
        m_spec.logicalDevice->as<vk::Device>().freeMemory(deviceMemoryAs<vk::DeviceMemory>());
    }
}

} // namespace R3

#endif // R3_VULKAN