#if R3_VULKAN

#include "render/TextureBuffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vulkan/vulkan.hpp>
#include "render/CommandPool.hpp"
#include "render/Image.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Queue.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

TextureBuffer::TextureBuffer(const TextureBufferSpecification& spec)
    : m_logicalDevice(spec.logicalDevice),
      m_type(spec.type) {
    CHECK((spec.path != nullptr) ^ (spec.data != nullptr));

    int32 w, h, channels;
    uint32 len = spec.height ? spec.width * spec.height : spec.width;
    const uint8* bytes = spec.data ? stbi_load_from_memory((const stbi_uc*)spec.data, len, &w, &h, &channels, 0)
                                   : stbi_load(spec.path, &w, &h, &channels, 0);
    CHECK(bytes != nullptr);
    const usize imageSize = w * h * 4;

    // staging buffer for wriring
    const BufferAllocateSpecification bufferAllocateSpecification = {
        .physicalDevice = *spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .size = imageSize,
        .bufferFlags = uint32(vk::BufferUsageFlagBits::eTransferSrc),
        .memoryFlags = uint32(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent),
    };
    auto [stagingBuffer, stagingMemory] = Buffer::allocate(bufferAllocateSpecification);

    // copy image data to buffer
    void* data = m_logicalDevice->as<vk::Device>().mapMemory(stagingMemory.as<vk::DeviceMemory>(), 0, imageSize, {});
    {
        // no Alpha channel
        if (channels == 3) {
            uint32* rgba = reinterpret_cast<uint32*>(data);
            constexpr uint32 OPAQUE_MASK = 0xFF00'0000;
            const uint32 rgbSize = w * h * 3;

            for (uint32 i = 0, j = 0; i < rgbSize; i += 3, j++) {
                // we overide the alpha channel with opaque mask 0xFF
                rgba[j] = (*(reinterpret_cast<const uint32*>(&bytes[i])) | OPAQUE_MASK);
            }
        }
        // has Alpha Channel
        else {
            memcpy(data, bytes, imageSize);
        }
    }
    m_logicalDevice->as<vk::Device>().unmapMemory(stagingMemory.as<vk::DeviceMemory>());
    stbi_image_free((void*)bytes);

    // real image we use to store data
    const ImageAllocateSpecification imageAllocateSpecification = {
        .physicalDevice = *spec.physicalDevice,
        .logicalDevice = *spec.logicalDevice,
        .size = imageSize,
        .format = R3_FORMAT_R8G8B8A8_SRGB,
        .width = static_cast<uint32>(w),
        .height = static_cast<uint32>(h),
        .imageFlags = uint32(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled),
        .memoryFlags = uint32(vk::MemoryPropertyFlagBits::eDeviceLocal),
    };
    auto [image, memory] = Image::allocate(imageAllocateSpecification);

    const auto& commandBuffer = spec.commandPool->commandBuffers().front();

    vk::ImageMemoryBarrier imageMemoryBarrier = {
        .sType = vk::StructureType::eImageMemoryBarrier,
        .pNext = nullptr,
        .srcAccessMask = {},
        .dstAccessMask = vk::AccessFlagBits::eTransferWrite,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eTransferDstOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = image.as<vk::Image>(),
        .subresourceRange =
            {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };
    commandBuffer.beginCommandBuffer(CommandBufferFlags::OneTimeSubmit);
    commandBuffer.as<vk::CommandBuffer>().pipelineBarrier({vk::PipelineStageFlagBits::eTopOfPipe},
                                                          {vk::PipelineStageFlagBits::eTransfer},
                                                          {},
                                                          {},
                                                          {},
                                                          {imageMemoryBarrier});
    commandBuffer.endCommandBuffer();
    commandBuffer.oneTimeSubmit();

    const ImageCopySpecification imageCopySpecification = {
        .logicalDevice = *m_logicalDevice,
        .commandPool = *spec.commandPool,
        .dst = image,
        .src = stagingBuffer,
        .size = imageSize,
        .width = static_cast<uint32>(w),
        .height = static_cast<uint32>(h),
        .copyType = ImageCopyType::BufferToImage,
    };
    Image::copy(imageCopySpecification);

    imageMemoryBarrier.oldLayout = imageMemoryBarrier.newLayout;
    imageMemoryBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    commandBuffer.beginCommandBuffer(CommandBufferFlags::OneTimeSubmit);
    commandBuffer.as<vk::CommandBuffer>().pipelineBarrier({vk::PipelineStageFlagBits::eTransfer},
                                                          {vk::PipelineStageFlagBits::eFragmentShader},
                                                          {},
                                                          {},
                                                          {},
                                                          {imageMemoryBarrier});
    commandBuffer.endCommandBuffer();
    commandBuffer.oneTimeSubmit();

    m_logicalDevice->as<vk::Device>().destroyBuffer(stagingBuffer.as<vk::Buffer>());
    m_logicalDevice->as<vk::Device>().freeMemory(stagingMemory.as<vk::DeviceMemory>());

    setHandle(image.handle());
    setDeviceMemory(memory.handle());

    const Image img(handle());
    m_imageView = ImageView(ImageViewSpecification{
        .logicalDevice = m_logicalDevice,
        .image = &img,
        .format = R3_FORMAT_R8G8B8A8_SRGB,
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    });

    m_sampler = Sampler(SamplerSpecification{
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = m_logicalDevice,
    });
}

TextureBuffer::~TextureBuffer() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyImage(as<vk::Image>());
        m_logicalDevice->as<vk::Device>().freeMemory(deviceMemoryAs<vk::DeviceMemory>());
    }
}

} // namespace R3

#endif // R3_VULKAN
