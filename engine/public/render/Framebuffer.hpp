#pragma once 

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct FramebufferSpecification {
    Ref<const LogicalDevice> logicalDevice;
    Ref<const Swapchain> swapchain;
    Ref<const ImageView> imageView;
    Ref<const RenderPass> renderPass;
};

class Framebuffer : public NativeRenderObject {
public:
    Framebuffer() = default;
    Framebuffer(const FramebufferSpecification& spec);
    Framebuffer(Framebuffer&&) noexcept = default;
    Framebuffer& operator=(Framebuffer&&) noexcept = default;
    ~Framebuffer();

private:
    FramebufferSpecification m_spec;
};

} // namespace R3