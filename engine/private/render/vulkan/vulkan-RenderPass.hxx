////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-RenderPass.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Types.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief ColorAttachment Specification.
struct ColorAttachmentSpecification {
    VkFormat format;                   ///< Color Attachment format.
    VkSampleCountFlagBits sampleCount; ///< MSAA samples.
};

/// @brief DepthAttachment Specification.
struct DepthAttachmentSpecification {
    VkSampleCountFlagBits sampleCount; ///< Max supported sample count.
};

/// @brief Vulkan RenderPass Specification.
struct RenderPassSpecification {
    const PhysicalDevice& physicalDevice;                ///< Valid PhysicalDevice.
    const LogicalDevice& device;                         ///< Valid LogicalDevice.
    const ColorAttachmentSpecification& colorAttachment; ///< ColorAttachmentSpecification.
    const DepthAttachmentSpecification& depthAttachment; ///< DepthAttachmentSpecification.
};

/// @brief Vulkan RenderPass RAII wrapper.
/// RenderPass describes the color and depth buffers and sampling for rendering.
class RenderPass : public VulkanObject<VkRenderPass> {
public:
    DEFAULT_CONSTRUCT(RenderPass);
    NO_COPY(RenderPass);
    DEFAULT_MOVE(RenderPass);

    /// @brief Create RenderPass, antialiasing is automatically MSAA (TODO).
    /// @param spec
    RenderPass(const RenderPassSpecification& spec);

    /// @brief Destroy RenderPass.
    ~RenderPass();

private:
    VkDevice m_device = VK_NULL_HANDLE;
};

} // namespace R3::vulkan

#endif // R3_VULKAN