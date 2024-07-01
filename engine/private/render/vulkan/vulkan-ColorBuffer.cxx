#if R3_VULKAN

#include "vulkan-ColorBuffer.hxx"

#include "vulkan-AttachmentBuffer.hxx"
#include <api/Result.hpp>
#include <type_traits>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<ColorBuffer> ColorBuffer::create(const ColorBufferSpecification& spec) {
    Result<AttachmentBuffer> parent = AttachmentBuffer::create({
        .physicalDevice = spec.physicalDevice,
        .device         = spec.device,
        .format         = spec.format,
        .extent         = spec.extent,
        .sampleCount    = spec.sampleCount,
        .aspectFlags    = VK_IMAGE_ASPECT_COLOR_BIT,
        .imageUsage     = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        .mipLevels      = 1,
    });
    R3_PROPAGATE(parent);
    return ColorBuffer(std::move(parent.value()));
}

} // namespace R3::vulkan

#endif // R3_VULKAN
