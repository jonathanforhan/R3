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
    const std::vector<vk::Image> swapchainImages =
        spec.logicalDevice.as<vk::Device>().getSwapchainImagesKHR(spec.swapchain.as<vk::SwapchainKHR>());

    std::vector<Image> images(swapchainImages.size());

    for (uint32 i = 0; i < swapchainImages.size(); i++) {
        images[i].setHandle(swapchainImages[i]);
    }

    return images;
}

std::tuple<NativeRenderObject, NativeRenderObject> Image::allocate(const ImageAllocateSpecification& spec) {
    CHECK(spec.mipLevels != 0);

    const uint32 indices[] = {
        spec.logicalDevice.graphicsQueue().index(),
        spec.logicalDevice.presentationQueue().index(),
    };

    const vk::ImageCreateInfo imageCreateInfo = {
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
        .mipLevels = spec.mipLevels,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = vk::ImageUsageFlags(spec.imageFlags),
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = indices,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    const vk::Image image = spec.logicalDevice.as<vk::Device>().createImage(imageCreateInfo);

    const auto memoryRequirements = spec.logicalDevice.as<vk::Device>().getImageMemoryRequirements(image);

    const vk::MemoryAllocateInfo memoryAllocateInfo = {
        .sType = vk::StructureType::eMemoryAllocateInfo,
        .pNext = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex =
            spec.physicalDevice.queryMemoryType(memoryRequirements.memoryTypeBits, (uint32)spec.memoryFlags),
    };
    const vk::DeviceMemory memory = spec.logicalDevice.as<vk::Device>().allocateMemory(memoryAllocateInfo);

    spec.logicalDevice.as<vk::Device>().bindImageMemory(image, memory, 0);

    return {static_cast<VkImage>(image), static_cast<VkDeviceMemory>(memory)};
}

void Image::copy(const ImageCopySpecification& spec) {
    const auto& commandBuffer = spec.commandPool.commandBuffers().front();

    commandBuffer.beginCommandBuffer(CommandBufferFlags::OneTimeSubmit);

    if (spec.copyType == ImageCopyType::BufferToImage) {
        const vk::BufferImageCopy bufferImageCopy = {
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

        commandBuffer.as<vk::CommandBuffer>().copyBufferToImage(spec.src.as<vk::Buffer>(),
                                                                spec.dst.as<vk::Image>(),
                                                                vk::ImageLayout::eTransferDstOptimal,
                                                                {bufferImageCopy});
    } else if (spec.copyType == ImageCopyType::Image) {
        const vk::ImageCopy imageCopy = {
            .srcSubresource =
                {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            .srcOffset = {0},
            .dstSubresource =
                {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            .dstOffset = {0},
            .extent =
                {
                    .width = spec.width,
                    .height = spec.height,
                    .depth = 1,
                },
        };
        commandBuffer.as<vk::CommandBuffer>().copyImage(spec.src.as<vk::Image>(),
                                                        vk::ImageLayout::eTransferSrcOptimal,
                                                        spec.dst.as<vk::Image>(),
                                                        vk::ImageLayout::eTransferDstOptimal,
                                                        imageCopy);
    }

    commandBuffer.endCommandBuffer();
    commandBuffer.oneTimeSubmit();
}

void Image::transition(const ImageLayoutTransitionSpecification& spec) {
    const auto& commandBuffer = spec.commandPool.commandBuffers().front();

    vk::ImageMemoryBarrier imageMemoryBarrier = {
        .sType = vk::StructureType::eImageMemoryBarrier,
        .pNext = nullptr,
        .srcAccessMask = vk::AccessFlags(spec.srcAccessor),
        .dstAccessMask = vk::AccessFlags(spec.dstAccessor),
        .oldLayout = vk::ImageLayout(spec.oldLayout),
        .newLayout = vk::ImageLayout(spec.newLayout),
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = spec.image.as<vk::Image>(),
        .subresourceRange =
            {
                .aspectMask = vk::ImageAspectFlags(spec.aspectMask),
                .baseMipLevel = spec.baseMipLevel,
                .levelCount = spec.mipLevels,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    commandBuffer.beginCommandBuffer(CommandBufferFlags::OneTimeSubmit);
    commandBuffer.as<vk::CommandBuffer>().pipelineBarrier({vk::PipelineStageFlags(spec.srcStageMask)},
                                                          {vk::PipelineStageFlags(spec.dstStageMask)},
                                                          {},
                                                          {},
                                                          {},
                                                          {imageMemoryBarrier});
    commandBuffer.endCommandBuffer();
    commandBuffer.oneTimeSubmit();
}

void Image::generateMipMaps(const ImageMipmapSpecification& spec) {
    const auto& commandBuffer = spec.commandPool.commandBuffers().front();

    ImageLayoutTransitionSpecification imageLayoutTransitionSpecificationWrite = {
        .commandPool = spec.commandPool,
        .image = spec.image,
        .srcAccessor = MemoryAccessor::TransferWrite,
        .dstAccessor = MemoryAccessor::TransferRead,
        .oldLayout = ImageLayout::TransferDstOptimal,
        .newLayout = ImageLayout::TransferSrcOptimal,
        .aspectMask = ImageAspect::Color,
        .mipLevels = 1,
        .baseMipLevel = 0,
        .srcStageMask = PipelineStage::Transfer,
        .dstStageMask = PipelineStage::Transfer,
    };

    ImageLayoutTransitionSpecification imageLayoutTransitionSpecificationShader = {
        .commandPool = spec.commandPool,
        .image = spec.image,
        .srcAccessor = MemoryAccessor::TransferRead,
        .dstAccessor = MemoryAccessor::ShaderRead,
        .oldLayout = ImageLayout::TransferSrcOptimal,
        .newLayout = ImageLayout::ShaderReadOnlyOptimal,
        .aspectMask = ImageAspect::Color,
        .mipLevels = 1,
        .baseMipLevel = 0,
        .srcStageMask = PipelineStage::Transfer,
        .dstStageMask = PipelineStage::FragmentShader,
    };

    int32 w = spec.width, h = spec.height;

    for (uint32 i = 0; i < spec.mipLevels - 1; i++) {
        imageLayoutTransitionSpecificationWrite.baseMipLevel = i;
        transition(imageLayoutTransitionSpecificationWrite);

        vk::ImageBlit blit = {
            .srcSubresource =
                {
                    .aspectMask = vk::ImageAspectFlags(ImageAspect::Color),
                    .mipLevel = i,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            .srcOffsets = {},
            .dstSubresource =
                {
                    .aspectMask = vk::ImageAspectFlags(ImageAspect::Color),
                    .mipLevel = i + 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            .dstOffsets = {},
        };

        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {w, h, 1};

        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {w > 1 ? w / 2 : 1, h > 1 ? h / 2 : 1, 1};

        commandBuffer.beginCommandBuffer(CommandBufferFlags::OneTimeSubmit);
        commandBuffer.as<vk::CommandBuffer>().blitImage(spec.image.as<vk::Image>(),
                                                        vk::ImageLayout::eTransferSrcOptimal,
                                                        spec.image.as<vk::Image>(),
                                                        vk::ImageLayout::eTransferDstOptimal,
                                                        blit,
                                                        vk::Filter::eLinear);
        commandBuffer.endCommandBuffer();
        commandBuffer.oneTimeSubmit();

        imageLayoutTransitionSpecificationShader.baseMipLevel = i;
        transition(imageLayoutTransitionSpecificationShader);

        w = w > 1 ? w / 2 : w;
        h = h > 1 ? h / 2 : h;
    }

    ImageLayoutTransitionSpecification imageLayoutTransitionSpecificationFinal = {
        .commandPool = spec.commandPool,
        .image = spec.image,
        .srcAccessor = MemoryAccessor::TransferWrite,
        .dstAccessor = MemoryAccessor::ShaderRead,
        .oldLayout = ImageLayout::TransferDstOptimal,
        .newLayout = ImageLayout::ShaderReadOnlyOptimal,
        .aspectMask = ImageAspect::Color,
        .mipLevels = 1,
        .baseMipLevel = spec.mipLevels - 1,
        .srcStageMask = PipelineStage::Transfer,
        .dstStageMask = PipelineStage::FragmentShader,
    };
    transition(imageLayoutTransitionSpecificationFinal);
}

} // namespace R3

#endif // R3_VULKAN