#pragma once

#include "render/Buffer.hpp"
#include "render/RenderFwd.hpp"
#include "render/ImageView.hpp"
#include "render/Sampler.hpp"

namespace R3 {

struct DepthBufferSpecification {
    Ref<const PhysicalDevice> physicalDevice;
    Ref<const LogicalDevice> logicalDevice;
    Ref<const Swapchain> swapchain;
    Ref<const CommandPool> commandPool;
};

class DepthBuffer : public Buffer {
public:
    DepthBuffer() = default;
    DepthBuffer(const DepthBufferSpecification& spec);
    DepthBuffer(DepthBuffer&&) noexcept = default;
    DepthBuffer& operator=(DepthBuffer&&) noexcept = default;
    ~DepthBuffer();

private:
    DepthBufferSpecification m_spec;
    ImageView m_imageView;
};

} // namespace R3
