#if R3_VULKAN

#include "render/Image.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/CommandPool.hpp"
#include "render/DeviceMemory.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

std::vector<Image> Image::acquireImages(const ImageSpecification& spec) {
    std::vector<vk::Image> swapchainImages =
        spec.logicalDevice->as<vk::Device>().getSwapchainImagesKHR(spec.swapchain->as<vk::SwapchainKHR>());

    std::vector<Image> images(swapchainImages.size());

    for (uint32 i = 0; i < swapchainImages.size(); i++) {
        images[i].setHandle(swapchainImages[i]);
    }

    return images;
}

std::tuple<Image::Handle, DeviceMemory::Handle> Image::allocate(const ImageAllocateSpecification& spec) {
    uint32 indices[]{
        spec.logicalDevice.graphicsQueue().index(),
        spec.logicalDevice.presentationQueue().index(),
    };

    vk::ImageCreateInfo imageCreateInfo = {
        .sType = vk::StructureType::eImageCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .imageType = vk::ImageType::e2D,
        .format = vk::Format(spec.format),
        .extent =
            {
                .width = spec.width,
                .height = spec.height,
                .depth = 1,
            },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = (vk::ImageUsageFlags) static_cast<uint32>(spec.imageFlags),
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = indices,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    VkImage image = spec.logicalDevice.as<vk::Device>().createImage(imageCreateInfo);

    auto memoryRequirements = spec.logicalDevice.as<vk::Device>().getImageMemoryRequirements(image);

    vk::MemoryAllocateInfo memoryAllocateInfo = {
        .sType = vk::StructureType::eMemoryAllocateInfo,
        .pNext = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex =
            spec.physicalDevice.queryMemoryType(memoryRequirements.memoryTypeBits, (uint32)spec.memoryFlags),
    };
    VkDeviceMemory memory = spec.logicalDevice.as<vk::Device>().allocateMemory(memoryAllocateInfo);

    spec.logicalDevice.as<vk::Device>().bindImageMemory(image, memory, 0);

    return {image, memory};
}

void Image::copy(const ImageCopySpecification& spec) {
    auto& commandBuffer = spec.commandPool.commandBuffers().front();

    commandBuffer.beginCommandBuffer(CommandBufferFlags::OneTimeSubmit);

    if (spec.copyType == ImageCopyType::BufferToImage) {
        vk::BufferImageCopy bufferImageCopy = {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource =
                {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            .imageOffset = {0, 0, 0},
            .imageExtent =
                {
                    .width = spec.width,
                    .height = spec.height,
                    .depth = 1,
                },
        };

        commandBuffer.as<vk::CommandBuffer>().copyBufferToImage(
            (VkBuffer)spec.src.get(), (VkImage)spec.dst.get(), vk::ImageLayout::eTransferDstOptimal, {bufferImageCopy});
    } else if (spec.copyType == ImageCopyType::Image) {
        vk::ImageCopy imageCopy = {
            .srcSubresource =
                {
                    .aspectMask = vk::ImageAspectFlagBits::eNone,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            .srcOffset = 0,
            .dstSubresource =
                {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            .dstOffset = 0,
            .extent =
                {
                    .width = spec.width,
                    .height = spec.height,
                    .depth = 1,
                },
        };
        commandBuffer.as<vk::CommandBuffer>().copyImage((VkImage)spec.src.get(),
                                                        vk::ImageLayout::eTransferSrcOptimal,
                                                        (VkImage)spec.dst.get(),
                                                        vk::ImageLayout::eTransferDstOptimal,
                                                        {imageCopy});
    }

    commandBuffer.endCommandBuffer();
}

} // namespace R3

#endif // R3_VULKAN