#pragma once

#include <string_view>
#include "render/Buffer.hpp"
#include "render/RenderFwd.hpp"
#include "render/ImageView.hpp"
#include "render/Sampler.hpp"

namespace R3 {

// clang-format off
enum class TextureType : uint8 {
    Albedo              = 0,
    Normal              = 1,
    MetallicRoughness   = 2,
    AmbientOcclusion    = 3,
    Emissive            = 4,
};
// clang-format on

struct TextureBufferSpecification {
    Ref<const PhysicalDevice> physicalDevice;
    Ref<const LogicalDevice> logicalDevice;
    Ref<const Swapchain> swapchain;
    Ref<const CommandPool> commandPool;
    uint32 width;
    uint32 height;
    const void* data;
    const char* path;
    TextureType type;
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