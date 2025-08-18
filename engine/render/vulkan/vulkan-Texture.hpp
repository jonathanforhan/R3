#pragma once

#include <vulkan/vulkan_core.h>
#include "Types.hpp"
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
    void create(RenderContext& ctx, VkCommandBuffer cmd, const uint8* raw, usize width, usize height, TextureType type);

    void create(RenderContext& ctx, VkCommandBuffer cmd, const uint8* compressed, usize size, TextureType type);

    void create(RenderContext& ctx, VkCommandBuffer cmd, const char* path, TextureType type);

    void destroy() noexcept;

    VkImage image() const noexcept { return m_image.image(); }

    VkImageView imageView() const noexcept { return m_image.imageView(); }

    VkSampler sampler() const noexcept { return m_sampler; }

    TextureType type() const noexcept { return m_type; }

private:
    VkDevice m_device   = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;
    Image m_image;
    TextureType m_type = TextureType::Albedo;
};

} // namespace R3::vulkan
