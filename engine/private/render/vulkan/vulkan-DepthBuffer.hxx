////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-DepthBuffer.hxx
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
protected:
    /// @brief Construct DepthBuffer from a moved AttachmentBuffer, needed for inheritance with fallible constructor.
    /// @param parent Valid AttachmentBuffer created with AttachmentBuffer::create
    DepthBuffer(AttachmentBuffer&& parent)
        : AttachmentBuffer(std::move(parent)) {}

public:
    DEFAULT_CONSTRUCT(DepthBuffer);
    NO_COPY(DepthBuffer);
    DEFAULT_MOVE(DepthBuffer);

    /// @brief Construct DepthBuffer.
    /// @return  DepthBuffer | UnsupportedFeature | InitializationFailure | AllocationFailure
    static Result<DepthBuffer> create(const DepthBufferSpecification& spec);

    /// @brief Query PhysicalDevice for supported formats.
    /// If PhysicalDevice does no support any R3 formats it is an error.
    /// @param physicalDevice
    /// @param tiling Desired tiling
    /// @param features Desired features
    /// @return Format that supports features | UnsupportedFeature
    static Result<VkFormat> querySupportedDepthFormat(VkPhysicalDevice physicalDevice,
                                                      VkImageTiling tiling,
                                                      VkFormatFeatureFlags features);
};

} // namespace R3::vulkan

#endif // R3_VULKAN