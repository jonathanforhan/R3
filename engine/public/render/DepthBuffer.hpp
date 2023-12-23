#pragma once

#include "render/Buffer.hpp"
#include "render/ImageView.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct DepthBufferSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    const Swapchain& swapchain;
};

class DepthBuffer : public Buffer {
public:
    DepthBuffer() = default;
    DepthBuffer(const DepthBufferSpecification& spec);
    DepthBuffer(DepthBuffer&&) noexcept = default;
    DepthBuffer& operator=(DepthBuffer&&) noexcept = default;
    ~DepthBuffer();

    const ImageView& imageView() const { return m_imageView; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    ImageView m_imageView;
};

} // namespace R3
