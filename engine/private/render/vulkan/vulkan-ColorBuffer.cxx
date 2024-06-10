#if R3_VULKAN

#include "vulkan-ColorBuffer.hxx"

#include "vulkan/vulkan.h"
#include "vulkan-AttachmentBuffer.hxx"
#include "vulkan-VulkanObject.hxx"

namespace R3::vulkan {

ColorBuffer::ColorBuffer(const ColorBufferSpecification& spec)
    : AttachmentBuffer({
          .physicalDevice = spec.physicalDevice,
          .device         = spec.device,
          .format         = spec.format,
          .extent         = spec.extent,
          .sampleCount    = spec.sampleCount,
          .aspectFlags    = VK_IMAGE_ASPECT_COLOR_BIT,
          .imageUsage     = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
          .memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
          .mipLevels      = 1,
      }) {}

} // namespace R3::vulkan

#endif // R3_VULKAN
