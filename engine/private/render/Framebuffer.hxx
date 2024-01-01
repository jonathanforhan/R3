#pragma once

/// A Framebuffer references all ImageViews that represent attachments like Color Attachment

#include <span>
#include "render/RenderApi.hxx"

namespace R3 {

/// @brief Framebuffer Specification
struct FramebufferSpecification {
    const LogicalDevice& logicalDevice;
    const RenderPass& renderPass;
    std::span<const ImageView*> attachments; ///< Image View attachments
    uvec2 extent;                            ///< Framebuffer extent
};

/// @brief Framebuffer is created per Image in Swapchain
/// Framebuffers are owned by the Swapchain and recreated when resize event triggers
class Framebuffer : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(Framebuffer);
    NO_COPY(Framebuffer);
    DEFAULT_MOVE(Framebuffer);

    /// @brief Construct Framebuffer from spec
    /// @param spec
    Framebuffer(const FramebufferSpecification& spec);

    /// @brief Destroy Framebuffer (ImageViews are not destroyed by Framebuffer)
    ~Framebuffer();

    /// @brief Query Current extent
    /// @return extent
    [[nodiscard]] constexpr uvec2 extent() const { return m_extent; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    uvec2 m_extent = uvec2(0);
};

} // namespace R3