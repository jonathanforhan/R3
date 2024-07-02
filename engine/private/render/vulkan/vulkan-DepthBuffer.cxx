#if R3_VULKAN

#include "vulkan-AttachmentBuffer.hxx"
#include "vulkan-DepthBuffer.hxx"
#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <expected>
#include <type_traits>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<DepthBuffer> DepthBuffer::create(const DepthBufferSpecification& spec) {
    DepthBuffer self;

    Result<VkFormat> format = querySupportedDepthFormat(
        spec.physicalDevice.vk(), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    R3_PROPAGATE(format);

    const uint32 indices[] = {
        spec.device.graphicsQueue().index(),
        spec.device.presentationQueue().index(),
    };

    const VkImageCreateInfo imageCreateInfo = {
        .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext     = nullptr,
        .flags     = {},
        .imageType = VK_IMAGE_TYPE_2D,
        .format    = format.value(),
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
        .usage                 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
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
        .format   = format.value(),
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
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

Result<VkFormat> DepthBuffer::querySupportedDepthFormat(VkPhysicalDevice physicalDevice,
                                                        VkImageTiling tiling,
                                                        VkFormatFeatureFlags features) {
    static constexpr VkFormat formats[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
    };

    for (VkFormat format : formats) {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

        switch (tiling) {
            case VK_IMAGE_TILING_LINEAR:
                if ((formatProperties.linearTilingFeatures & features) == features) {
                    return format;
                }
                break;
            case VK_IMAGE_TILING_OPTIMAL:
                if ((formatProperties.optimalTilingFeatures & features) == features) {
                    return format;
                }
                break;
            default:
                continue;
        }
    }

    R3_LOG(Error, "GPU does not support required depth formats");
    return std::unexpected(Error::UnsupportedFeature);
}

} // namespace R3::vulkan

#endif // R3_VULKAN
