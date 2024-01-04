#pragma once

#include "render/RenderApi.hpp"

namespace R3 {

struct R3_API ColorAttachmentSpecification {
    Format format;     ///< Color Attachment format
    uint8 sampleCount; ///< MSAA samples
};

struct R3_API DepthAttachmentSpecification {
    uint8 sampleCount; ///< MSAA samples
};

/// @brief Render Pass Specification
struct R3_API RenderPassSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    const ColorAttachmentSpecification& colorAttachment;
    const DepthAttachmentSpecification& depthAttachment;
};

/// @brief RenderPass represents a collection of attachments, subpasses and subpass dependencies
class R3_API RenderPass : public NativeRenderObject {
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