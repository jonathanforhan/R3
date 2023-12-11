#pragma once

#include <string_view>
#include "render/Buffer.hpp"
#include "render/RenderFwd.hpp"
#include "render/ImageView.hpp"
#include "render/Sampler.hpp"

namespace R3 {

struct TextureBufferSpecification {
    Ref<const PhysicalDevice> physicalDevice;
    Ref<const LogicalDevice> logicalDevice;
    Ref<const Swapchain> swapchain;
    Ref<const CommandPool> commandPool;
    std::string_view path;
};

class TextureBuffer : public Buffer {
public:
    TextureBuffer() = default;
    TextureBuffer(const TextureBufferSpecification& spec);
    TextureBuffer(TextureBuffer&&) noexcept = default;
    TextureBuffer& operator=(TextureBuffer&&) noexcept = default;
    ~TextureBuffer();

    const ImageView& textureView() const { return m_imageView; }
    const Sampler& sampler() const { return m_sampler; }

private:
    TextureBufferSpecification m_spec;
    ImageView m_imageView;
    Sampler m_sampler;
};

} // namespace R3