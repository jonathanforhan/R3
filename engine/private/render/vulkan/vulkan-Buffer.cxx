#if R3_VULKAN

#include "render/Buffer.hpp"

#include <vulkan/vulkan.hpp>
#include "render/CommandPool.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"

namespace R3 {

std::tuple<NativeRenderObject, NativeRenderObject> Buffer::allocate(const BufferAllocateSpecification& spec) {
    const uint32 indices[] = {
        spec.logicalDevice.graphicsQueue().index(),
        spec.logicalDevice.presentationQueue().index(),
    };

    const vk::BufferCreateInfo bufferCreateInfo = {
        .sType = vk::StructureType::eBufferCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .size = spec.size,
        .usage = vk::BufferUsageFlags(spec.bufferFlags),
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = indices,
    };
    const auto buffer = spec.logicalDevice.as<vk::Device>().createBuffer(bufferCreateInfo);

    const auto memoryRequirements = spec.logicalDevice.as<vk::Device>().getBufferMemoryRequirements(buffer);

    const vk::MemoryAllocateInfo memoryAllocateInfo = {
        .sType = vk::StructureType::eMemoryAllocateInfo,
        .pNext = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = spec.physicalDevice.queryMemoryType(memoryRequirements.memoryTypeBits, spec.memoryFlags),
    };
    const auto memory = spec.logicalDevice.as<vk::Device>().allocateMemory(memoryAllocateInfo);

    spec.logicalDevice.as<vk::Device>().bindBufferMemory(buffer, memory, 0);

    return {static_cast<VkBuffer>(buffer), static_cast<VkDeviceMemory>(memory)};
}

void Buffer::copy(const BufferCopySpecification& spec) {
    spec.commandBuffer.beginCommandBuffer(CommandBufferUsage::OneTimeSubmit);
    spec.commandBuffer.as<vk::CommandBuffer>().copyBuffer(
        spec.stagingBuffer.as<vk::Buffer>(), spec.buffer.as<vk::Buffer>(), {{.size = spec.size}});
    spec.commandBuffer.endCommandBuffer();

    const vk::CommandBuffer buffers[] = {spec.commandBuffer.as<vk::CommandBuffer>()};
    const vk::SubmitInfo submitInfo = {
        .sType = vk::StructureType::eSubmitInfo,
        .commandBufferCount = 1,
        .pCommandBuffers = buffers,
    };

    spec.logicalDevice.graphicsQueue().as<vk::Queue>().submit(submitInfo);
    spec.logicalDevice.graphicsQueue().as<vk::Queue>().waitIdle();
}

} // namespace R3

#endif // R3_VULKAN