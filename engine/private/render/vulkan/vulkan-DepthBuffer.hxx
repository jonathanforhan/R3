////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-DepthBuffer.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "vulkan-AttachmentBuffer.hxx"
#include "vulkan-fwd.hxx"
#include <api/Construct.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief DepthBuffer Specification.
struct DepthBufferSpecification {
    const PhysicalDevice& physicalDevice; ///< Valid PhsyicalDevice.
    const LogicalDevice& device;          ///< Valid LogicalDevice.
    VkExtent2D extent;                    ///< Image extent.
    VkSampleCountFlagBits sampleCount;    ///< Image sample count.
};

/// @brief Vulkan DepthBuffer RAII wrapper.
/// Used in RenderPass.
class DepthBuffer : public AttachmentBuffer {
public:
    DEFAULT_CONSTRUCT(DepthBuffer);
    NO_COPY(DepthBuffer);
    DEFAULT_MOVE(DepthBuffer);

    /// @brief Construct DepthBuffer.
    /// @param spec
    DepthBuffer(const DepthBufferSpecification& spec);

    /// @brief Query PhysicalDevice for supported formats.
    /// If PhysicalDevice does no support any R3 formats it is a fatal error.
    /// @param physicalDevice
    /// @param tiling Desired tiling
    /// @param features Desired features
    /// @return Format that supports features
    static VkFormat querySupportedDepthFormat(VkPhysicalDevice physicalDevice,
                                              VkImageTiling tiling,
                                              VkFormatFeatureFlags features);
};

} // namespace R3::vulkan

#endif // R3_VULKAN
