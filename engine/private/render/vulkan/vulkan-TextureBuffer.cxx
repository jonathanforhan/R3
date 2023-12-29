#if R3_VULKAN

#include "render/TextureBuffer.hxx"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/CommandPool.hxx"
#include "render/Image.hxx"
#include "render/LogicalDevice.hxx"
#include "render/PhysicalDevice.hxx"
#include "render/Queue.hxx"
#include "render/Swapchain.hxx"

namespace R3 {

TextureBuffer::TextureBuffer(const TextureBufferSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice),
      m_type(spec.type) {
    CHECK((spec.path != nullptr) ^ (spec.data != nullptr) ^ (spec.raw != nullptr));

    // check for blitting capability
    [[maybe_unused]] const auto& gpu = spec.physicalDevice.as<vk::PhysicalDevice>();
    CHECK(gpu.getFormatProperties(vk::Format::eR8G8B8A8Srgb).optimalTilingFeatures &
          vk::FormatFeatureFlagBits::eSampledImageFilterLinear);

    // defaults
    int32 w = spec.width;
    int32 h = spec.height;
    int32 channels = 4;
    const int32 len = spec.height ? spec.width * spec.height : spec.width;

    const uint8* bytes = nullptr;

    if (spec.path) {
        // modifies defaults
        bytes = stbi_load(spec.path, &w, &h, &channels, 0);
    } else if (spec.data) {
        // modifies defaults
        bytes = stbi_load_from_memory((const stbi_uc*)spec.data, len, &w, &h, &channels, 0);
    } else if (spec.raw) {
        bytes = spec.raw;
    }

    CHECK(bytes != nullptr);
    const usize imageSize = usize(w * h * 4);
    auto mipLevels = static_cast<uint32>(std::floor(std::log2(std::max(w, h)))) + 1;

    // staging buffer for wriring
    const BufferAllocateSpecification bufferAllocateSpecification = {
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .size = imageSize,
        .bufferFlags = BufferUsage::TransferSrc,
        .memoryFlags = MemoryProperty::HostVisible | MemoryProperty::HostCoherent,
    };
    auto&& [stagingBuffer, stagingMemory] = Buffer::allocate(bufferAllocateSpecification);

    // copy image data to buffer
    void* data = m_logicalDevice->as<vk::Device>().mapMemory(stagingMemory.as<vk::DeviceMemory>(), 0, imageSize, {});
    {
        // no Alpha channel
        if (channels == 3) {
            uint32* rgba = reinterpret_cast<uint32*>(data);
            static constexpr uint32 OPAQUE_MASK = 0xFF00'0000;
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

    if (!spec.raw) {
        stbi_image_free((void*)bytes);
    }

    // real image we use to store data
    const ImageAllocateSpecification imageAllocateSpecification = {
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .size = imageSize,
        .format = Format::R8G8B8A8Srgb,
        .width = static_cast<uint32>(w),
        .height = static_cast<uint32>(h),
        .mipLevels = mipLevels,
        .samples = 1,
        .imageFlags = ImageUsage::TransferSrc | ImageUsage::TransferDst | ImageUsage::Sampled,
        .memoryFlags = MemoryProperty::DeviceLocal,
    };

    auto&& [img, memory] = Image::allocate(imageAllocateSpecification);
    Image image(img.handle());

    // transition image layout in pipeline
    const ImageLayoutTransitionSpecification imageLayoutTransitionSpecificationWrite = {
        .commandBuffer = spec.commandBuffer,
        .image = image,
        .srcAccessor = {},
        .dstAccessor = MemoryAccessor::TransferWrite,
        .oldLayout = ImageLayout::Undefined,
        .newLayout = ImageLayout::TransferDstOptimal,
        .aspectMask = ImageAspect::Color,
        .mipLevels = mipLevels,
        .srcStageMask = PipelineStage::TopOfPipe,
        .dstStageMask = PipelineStage::Transfer,
    };
    Image::transition(imageLayoutTransitionSpecificationWrite);

    const ImageCopySpecification imageCopySpecification = {
        .logicalDevice = *m_logicalDevice,
        .commandBuffer = spec.commandBuffer,
        .dst = image,
        .src = stagingBuffer,
        .size = imageSize,
        .width = static_cast<uint32>(w),
        .height = static_cast<uint32>(h),
        .copyType = ImageCopyType::BufferToImage,
    };
    Image::copy(imageCopySpecification);

    const ImageMipmapSpecification imageMipmapSpecification = {
        .commandBuffer = spec.commandBuffer,
        .image = image,
        .mipLevels = mipLevels,
        .width = w,
        .height = h,
    };
    Image::generateMipMaps(imageMipmapSpecification);

    m_logicalDevice->as<vk::Device>().destroyBuffer(stagingBuffer.as<vk::Buffer>());
    m_logicalDevice->as<vk::Device>().freeMemory(stagingMemory.as<vk::DeviceMemory>());

    setHandle(image.handle());
    setDeviceMemory(memory.handle());

    m_imageView = ImageView({
        .logicalDevice = *m_logicalDevice,
        .image = image,
        .format = Format::R8G8B8A8Srgb,
        .mipLevels = mipLevels,
        .aspectMask = ImageAspect::Color,
    });

    m_sampler = Sampler({
        .physicalDevice = spec.physicalDevice,
        .logicalDevice = *m_logicalDevice,
        .mipLevels = mipLevels,
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
