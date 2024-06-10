#pragma once

#if R3_VULKAN

#include "api/Construct.hpp"
#include "vulkan-AttachmentBuffer.hxx"
#include "vulkan-fwd.hxx"
#include <vulkan/vulkan.h>

namespace R3::vulkan {

struct ColorBufferSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& device;
    VkFormat format;
    VkExtent2D extent;
    VkSampleCountFlagBits sampleCount;
};

class ColorBuffer : public AttachmentBuffer {
public:
    DEFAULT_CONSTRUCT(ColorBuffer);
    NO_COPY(ColorBuffer);
    DEFAULT_MOVE(ColorBuffer);

    ColorBuffer(const ColorBufferSpecification& spec);
};

} // namespace R3::vulkan

#endif // R3_VULKAN