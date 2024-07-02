#if R3_VULKAN

#include "vulkan-ColorBuffer.hxx"

#include "vulkan-AttachmentBuffer.hxx"
#include "vulkan-LogicalDevice.hxx"
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<ColorBuffer> ColorBuffer::create(const ColorBufferSpecification& spec) {
    ColorBuffer self;

    const uint32 indices[] = {
        spec.device.graphicsQueue().index(),
        spec.device.presentationQueue().index(),
    };

    const VkImageCreateInfo imageCreateInfo = {
        .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext     = nullptr,
        .flags     = {},
        .imageType = VK_IMAGE_TYPE_2D,
        .format    = spec.format,
        .extent =
            {
                .width  = spec.extent.width,
                .height = spec.extent.height,
                .depth  = 1,
            },
        .mipLevels             = 1,
        .arrayLayers           = 1,
        .samples               = spec.sampleCount,
        .tiling                = VK_IMAGE_TILING_OPTIMAL,
        .usage                 = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = static_cast<uint32>(std::size(indices)),
        .pQueueFamilyIndices   = indices,
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VkImageViewCreateInfo imageViewCreateInfo = {
        .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = {},
        .image    = VK_NULL_HANDLE,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format   = spec.format,
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };

    Result<void> result = self.initialize({
        .physicalDevice      = spec.physicalDevice,
        .device              = spec.device,
        .imageCreateInfo     = imageCreateInfo,
        .imageViewCreateInfo = imageViewCreateInfo,
        .memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    });
    R3_PROPAGATE(result);

    return self;
}

} // namespace R3::vulkan

#endif // R3_VULKAN
