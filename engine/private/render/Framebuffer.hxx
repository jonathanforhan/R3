#pragma once

/// A Framebuffer references all ImageViews that represent attachments like Color Attachment

#include "render/RenderApi.hxx"

namespace R3 {

/// @brief Framebuffer Specification
struct FramebufferSpecification {
    const LogicalDevice& logicalDevice;    ///< LogicalDevice
    const Swapchain& swapchain;            ///< Swapchain
    const ImageView& swapchainImageView;   ///< Swapchain ImageView
    const ImageView& colorBufferImageView; ///< ColorBuffer ImageView
    const ImageView& depthBufferImageView; ///< DepthBuffer ImageView
    const RenderPass& renderPass;          ///< RenderPass
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

private:
    Ref<const LogicalDevice> m_logicalDevice;
};

} // namespace R3