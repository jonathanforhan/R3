#pragma once

#include <filesystem>
#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "api/Types.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-Image.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

enum class TextureType {
    Albedo            = 0,
    MetallicRoughness = 1,
    Normal            = 2,
    AmbientOcclusion  = 3,
    Emissive          = 4,
};

class Texture {
public:
    R3_CTOR_DEFAULT(Texture);
    R3_COPY_DELETE(Texture);
    R3_MOVE_DEFAULT(Texture);

    Texture(RenderContext& ctx, CommandBuffer& cmd, const uint8* raw, usize width, usize height, TextureType type);

    Texture(RenderContext& ctx, CommandBuffer& cmd, const uint8* compressed, usize size, TextureType type);

    Texture(RenderContext& ctx, CommandBuffer& cmd, const std::filesystem::path& filepath, TextureType type);

    ~Texture() noexcept;

    VkImage image() const noexcept { return m_image.image(); }

    VkImageView imageView() const noexcept { return m_image.imageView(); }

    VkSampler sampler() const noexcept { return m_sampler; }

    TextureType type() const noexcept { return m_type; }

private:
    void create(RenderContext& ctx, CommandBuffer& cmd, const uint8* raw, usize width, usize height, TextureType type);

    bool supportsBlitting(RenderContext& ctx, VkFormat format);

    VkFormat queryPreferredFormat(TextureType type) const noexcept;

private:
    Handle<VkDevice> m_device;
    Handle<VkSampler> m_sampler;
    Image m_image;
    TextureType m_type = TextureType::Albedo;
};

} // namespace R3::vulkan
