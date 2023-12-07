#if R3_VULKAN

#include "render/VertexBuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/CommandPool.hpp"

namespace R3 {

namespace local {

static auto createBuffer(const LogicalDevice* device,
                         const PhysicalDevice* physicalDevice,
                         vk::DeviceSize size,
                         vk::BufferUsageFlags usage,
                         vk::MemoryPropertyFlags properties) {
    uint32 indices[]{
        device->graphicsQueue().index(),
        device->presentationQueue().index(),
    };

    vk::BufferCreateInfo bufferCreateInfo = {
        .sType = vk::StructureType::eBufferCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .size = size,
        .usage = usage,
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = indices,
    };
    vk::Buffer buffer = device->as<vk::Device>().createBuffer(bufferCreateInfo);

    auto memoryRequirements = device->as<vk::Device>().getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo memoryAllocateInfo = {
        .sType = vk::StructureType::eMemoryAllocateInfo,
        .pNext = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = physicalDevice->queryMemoryType(memoryRequirements.memoryTypeBits, (uint32)properties),
    };
    auto memory = device->as<vk::Device>().allocateMemory(memoryAllocateInfo);

    device->as<vk::Device>().bindBufferMemory(buffer, memory, 0);

    return std::tuple<vk::Buffer, vk::DeviceMemory>(buffer, memory);
}

static void copyBuffer(vk::Buffer dst,
                       vk::Buffer src,
                       vk::DeviceSize size,
                       const LogicalDevice* logicalDevice,
                       const CommandPool* commandPool) {
    const auto& commandBuffer = commandPool->commandBuffers().front();
    commandBuffer.beginCommandBuffer(CommandBufferFlags::OneTimeSubmit);
    commandBuffer.as<vk::CommandBuffer>().copyBuffer(src, dst, {{.size = size}});
    commandBuffer.endCommandBuffer();

    vk::CommandBuffer buffers[] = {commandBuffer.as<vk::CommandBuffer>()};
    vk::SubmitInfo submitInfo = {
        .sType = vk::StructureType::eSubmitInfo,
        .commandBufferCount = 1,
        .pCommandBuffers = buffers,
    };

    logicalDevice->graphicsQueue().as<vk::Queue>().submit(submitInfo);
    logicalDevice->graphicsQueue().as<vk::Queue>().waitIdle();
}

} // namespace local

void VertexBuffer::create(const VertexBufferSpecification& spec) {
    CHECK(spec.physicalDevice != nullptr);
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.commandPool != nullptr);
    m_spec = spec;
    m_vertexCount = static_cast<uint32>(spec.vertices.size());

    // staging buffer, CPU writable
    auto [stagingBuffer, stagingMemory] =
        local::createBuffer(m_spec.logicalDevice,
                            m_spec.physicalDevice,
                            m_spec.vertices.size_bytes(),
                            vk::BufferUsageFlagBits::eTransferSrc,
                            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    // copy data to staging buffer
    void* data = m_spec.logicalDevice->as<vk::Device>().mapMemory(stagingMemory, 0, m_spec.vertices.size_bytes(), {});
    memcpy(data, m_spec.vertices.data(), m_spec.vertices.size_bytes());
    m_spec.logicalDevice->as<vk::Device>().unmapMemory(stagingMemory);

    // real buffer, copy staging buffer to this GPU buffer
    auto [buffer, memory] =
        local::createBuffer(m_spec.logicalDevice,
                            m_spec.physicalDevice,
                            m_spec.vertices.size_bytes(),
                            vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                            vk::MemoryPropertyFlagBits::eDeviceLocal);

    local::copyBuffer(buffer, stagingBuffer, m_spec.vertices.size_bytes(), m_spec.logicalDevice, m_spec.commandPool);
    m_spec.logicalDevice->as<vk::Device>().destroyBuffer(stagingBuffer);
    m_spec.logicalDevice->as<vk::Device>().freeMemory(stagingMemory);

    setHandle(buffer);
    setBuffer(memory);
}

void VertexBuffer::destroy() {
    m_spec.logicalDevice->as<vk::Device>().destroyBuffer(as<vk::Buffer>());
    m_spec.logicalDevice->as<vk::Device>().freeMemory(bufferAs<vk::DeviceMemory>());
}

} // namespace R3

#endif // R3_VULKAN