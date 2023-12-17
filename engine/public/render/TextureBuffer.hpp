#pragma once

#include <string_view>
#include "render/Buffer.hpp"
#include "render/ImageView.hpp"
#include "render/RenderFwd.hpp"
#include "render/Sampler.hpp"

namespace R3 {

// clang-format off
enum class TextureType : uint8 {
    Nil                 = 0,
    Albedo              = 1,
    Normal              = 2,
    MetallicRoughness   = 3,
    AmbientOcclusion    = 4,
    Emissive            = 5,
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

    TextureType type() const { return m_spec.type; }
    uint8 typeIndex() const { return (uint8)m_spec.type; }
    const ImageView& textureView() const { return m_imageView; }
    const Sampler& sampler() const { return m_sampler; }

private:
    TextureBufferSpecification m_spec;
    ImageView m_imageView;
    Sampler m_sampler;
};

} // namespace R3