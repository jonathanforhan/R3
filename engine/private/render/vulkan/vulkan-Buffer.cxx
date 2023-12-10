#if R3_VULKAN

#include "render/Buffer.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/CommandPool.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

std::tuple<Buffer::Handle, DeviceMemory::Handle> Buffer::allocate(const BufferAllocateSpecification& spec) {
    uint32 indices[]{
        spec.logicalDevice.graphicsQueue().index(),
        spec.logicalDevice.presentationQueue().index(),
    };

    vk::BufferCreateInfo bufferCreateInfo = {
        .sType = vk::StructureType::eBufferCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .size = spec.size,
        .usage = (vk::BufferUsageFlags) static_cast<uint32>(spec.bufferFlags),
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = indices,
    };
    VkBuffer buffer = spec.logicalDevice.as<vk::Device>().createBuffer(bufferCreateInfo);

    auto memoryRequirements = spec.logicalDevice.as<vk::Device>().getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo memoryAllocateInfo = {
        .sType = vk::StructureType::eMemoryAllocateInfo,
        .pNext = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex =
            spec.physicalDevice.queryMemoryType(memoryRequirements.memoryTypeBits, (uint32)spec.memoryFlags),
    };
    VkDeviceMemory memory = spec.logicalDevice.as<vk::Device>().allocateMemory(memoryAllocateInfo);

    spec.logicalDevice.as<vk::Device>().bindBufferMemory(buffer, memory, 0);

    return {buffer, memory};
}

void Buffer::copy(const BufferCopySpecification& spec) {
    const auto& commandBuffer = spec.commandPool.commandBuffers().front();
    commandBuffer.beginCommandBuffer(CommandBufferFlags::OneTimeSubmit);
    commandBuffer.as<vk::CommandBuffer>().copyBuffer(
        (VkBuffer)spec.stagingBuffer.get(), (VkBuffer)spec.buffer.get(), {{.size = spec.size}});
    commandBuffer.endCommandBuffer();

    vk::CommandBuffer buffers[] = {commandBuffer.as<vk::CommandBuffer>()};
    vk::SubmitInfo submitInfo = {
        .sType = vk::StructureType::eSubmitInfo,
        .commandBufferCount = 1,
        .pCommandBuffers = buffers,
    };

    spec.logicalDevice.graphicsQueue().as<vk::Queue>().submit(submitInfo);
    spec.logicalDevice.graphicsQueue().as<vk::Queue>().waitIdle();
}

} // namespace R3

#endif // R3_VULKAN