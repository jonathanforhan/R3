#if R3_VULKAN

#include "vulkan-AttachmentBuffer.hxx"
#include "vulkan-DepthBuffer.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include <api/Assert.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

DepthBuffer::DepthBuffer(const DepthBufferSpecification& spec)
    : AttachmentBuffer({
          .physicalDevice = spec.physicalDevice,
          .device         = spec.device,
          .format         = querySupportedDepthFormat(spec.physicalDevice.vk(),
                                              VK_IMAGE_TILING_OPTIMAL,
                                              VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT),
          .extent         = spec.extent,
          .sampleCount    = spec.sampleCount,
          .aspectFlags    = VK_IMAGE_ASPECT_DEPTH_BIT,
          .imageUsage     = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
          .memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
          .mipLevels      = 1,
      }) {}

VkFormat DepthBuffer::querySupportedDepthFormat(VkPhysicalDevice physicalDevice,
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
                if ((formatProperties.linearTilingFeatures & features) == features)
                    return format;
                break;
            case VK_IMAGE_TILING_OPTIMAL:
                if ((formatProperties.optimalTilingFeatures & features) == features)
                    return format;
                break;
            default:
                continue;
        }
    }

    ENSURE(false);
}

} // namespace R3::vulkan

#endif // R3_VULKAN
