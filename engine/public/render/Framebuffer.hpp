#pragma once 

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct FramebufferSpecification {
    const LogicalDevice* logicalDevice;
    const Swapchain* swapchain;
    const ImageView* imageView;
    const RenderPass* renderPass;
};

class Framebuffer : public NativeRenderObject {
public:
    void create(const FramebufferSpecification& spec);
    void destroy();

private:
    FramebufferSpecification m_spec;
};

} // namespace R3