/**
 * @file vulkan-ColorBuffer.hxx
 * @copyright GNU Public License
 */

#pragma once

#if R3_VULKAN

#include "vulkan-AttachmentBuffer.hxx"
#include "vulkan-fwd.hxx"
#include <api/Construct.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/**
 * ColorBuffer Specification.
 */
struct ColorBufferSpecification {
    const PhysicalDevice& physicalDevice; /**< Valid PhsyicalDevice. */
    const LogicalDevice& device;          /**< Valid LogicalDevice. */
    VkFormat format;                      /**< Image format. */
    VkExtent2D extent;                    /**< Image extent. */
    VkSampleCountFlagBits sampleCount;    /**< Image sample count. */
};

/**
 * Vulkan ColorBuffer RAII wrapper.
 * Used in RenderPass.
 */
class ColorBuffer : public AttachmentBuffer {
public:
    DEFAULT_CONSTRUCT(ColorBuffer);
    NO_COPY(ColorBuffer);
    DEFAULT_MOVE(ColorBuffer);

    /**
     * Construct a ColorBuffer.
     * @param spec
     */
    ColorBuffer(const ColorBufferSpecification& spec);
};

} // namespace R3::vulkan

#endif // R3_VULKAN