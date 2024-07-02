////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-ColorBuffer.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "vulkan-AttachmentBuffer.hxx"
#include "vulkan-fwd.hxx"
#include <api/Construct.hpp>
#include <api/Result.hpp>
#include <type_traits>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief ColorBuffer Specification.
struct ColorBufferSpecification {
    const PhysicalDevice& physicalDevice; ///< Valid PhsyicalDevice.
    const LogicalDevice& device;          ///< Valid LogicalDevice.
    VkFormat format;                      ///< Image format.
    VkExtent2D extent;                    ///< Image extent.
    VkSampleCountFlagBits sampleCount;    ///< Image sample count.
};

/// @brief Vulkan ColorBuffer RAII wrapper.
/// Used in RenderPass.
class ColorBuffer : public AttachmentBuffer {
public:
    DEFAULT_CONSTRUCT(ColorBuffer);
    NO_COPY(ColorBuffer);
    DEFAULT_MOVE(ColorBuffer);

    /// @brief Construct a ColorBuffer.
    /// @return  ColorBuffer | InitializationFailure | AllocationFailure
    static Result<ColorBuffer> create(const ColorBufferSpecification& spec);
};

} // namespace R3::vulkan

#endif // R3_VULKAN