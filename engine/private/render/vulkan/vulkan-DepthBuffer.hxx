#pragma once

#if R3_VULKAN

#include "api/Construct.hpp"
#include "vulkan-AttachmentBuffer.hxx"
#include "vulkan-fwd.hxx"
#include <vulkan/vulkan.h>

namespace R3::vulkan {

struct DepthBufferSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& device;
    VkExtent2D extent;
    VkSampleCountFlagBits sampleCount;
};

class DepthBuffer : public AttachmentBuffer {
public:
    DEFAULT_CONSTRUCT(DepthBuffer);
    NO_COPY(DepthBuffer);
    DEFAULT_MOVE(DepthBuffer);

    DepthBuffer(const DepthBufferSpecification& spec);

    static VkFormat querySupportedDepthFormat(VkPhysicalDevice physicalDevice,
                                              VkImageTiling tiling,
                                              VkFormatFeatureFlags features);
};

} // namespace R3::vulkan

#endif // R3_VULKAN
