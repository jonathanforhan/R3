#pragma once

#include "render/RenderApi.hxx"

namespace R3 {

struct ColorAttachmentSpecification {
    Format format;     ///< Color Attachment format
    uint8 sampleCount; ///< MSAA samples
};

struct DepthAttachmentSpecification {
    uint8 sampleCount; ///< MSAA samples
};

/// @brief Render Pass Specification
struct RenderPassSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    const ColorAttachmentSpecification& colorAttachment;
    const DepthAttachmentSpecification& depthAttachment;
};

/// @brief RenderPass represents a collection of attachments, subpasses and subpass dependencies
class RenderPass : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(RenderPass);
    NO_COPY(RenderPass);
    DEFAULT_MOVE(RenderPass);

    /// @brief Construct RenderPass from spec
    /// @param spec
    RenderPass(const RenderPassSpecification& spec);

    /// @brief Destroy RenderPass
    ~RenderPass();

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3