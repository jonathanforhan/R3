#if R3_VULKAN

#include "render/Buffer.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/CommandPool.hpp"

namespace R3 {

[[nodiscard]] std::tuple<Buffer::Handle, Buffer::Handle> Buffer::allocate(const LogicalDevice& logicalDevice,
                                                                          const PhysicalDevice& physicalDevice,
                                                                          usize size,
                                                                          uint64 bufferFlags,
                                                                          uint64 memoryFlags) {
    uint32 indices[]{
        logicalDevice.graphicsQueue().index(),
        logicalDevice.presentationQueue().index(),
    };

    vk::BufferCreateInfo bufferCreateInfo = {
        .sType = vk::StructureType::eBufferCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .size = size,
        .usage = (vk::BufferUsageFlags)static_cast<uint32>(bufferFlags),
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = indices,
    };
    VkBuffer buffer = logicalDevice.as<vk::Device>().createBuffer(bufferCreateInfo);

    auto memoryRequirements = logicalDevice.as<vk::Device>().getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo memoryAllocateInfo = {
        .sType = vk::StructureType::eMemoryAllocateInfo,
        .pNext = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = physicalDevice.queryMemoryType(memoryRequirements.memoryTypeBits, (uint32)memoryFlags),
    };
    VkDeviceMemory memory = logicalDevice.as<vk::Device>().allocateMemory(memoryAllocateInfo);

    logicalDevice.as<vk::Device>().bindBufferMemory(buffer, memory, 0);

    return {buffer, memory};
}

void Buffer::copy(Handle buffer,
                  const Handle stagingBuffer,
                  usize size,
                  const LogicalDevice& logicalDevice,
                  const CommandPool& commandPool) {
    CHECK(buffer != NULL);
    CHECK(stagingBuffer != NULL);

    const auto& commandBuffer = commandPool.commandBuffers().front();
    commandBuffer.beginCommandBuffer(CommandBufferFlags::OneTimeSubmit);
    commandBuffer.as<vk::CommandBuffer>().copyBuffer((VkBuffer)stagingBuffer, (VkBuffer)buffer, {{.size = size}});
    commandBuffer.endCommandBuffer();

    vk::CommandBuffer buffers[] = {commandBuffer.as<vk::CommandBuffer>()};
    vk::SubmitInfo submitInfo = {
        .sType = vk::StructureType::eSubmitInfo,
        .commandBufferCount = 1,
        .pCommandBuffers = buffers,
    };

    logicalDevice.graphicsQueue().as<vk::Queue>().submit(submitInfo);
    logicalDevice.graphicsQueue().as<vk::Queue>().waitIdle();
}

} // namespace R3

#endif // R3_VULKAN