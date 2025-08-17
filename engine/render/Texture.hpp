#pragma once

#include <vulkan/vulkan_core.h>
#include "Flags.hpp"
#include "Image.hpp"
#include "Types.hpp"

namespace R3 {

class RenderContext;

class Texture {
public:
    void create(RenderContext& ctx, VkCommandBuffer cmd, const uint8* raw, usize width, usize height, TextureType type);

    void create(RenderContext& ctx, VkCommandBuffer cmd, const uint8* compressed, usize size, TextureType type);

    void create(RenderContext& ctx, VkCommandBuffer cmd, const char* path, TextureType type);

    void destroy() noexcept;

    VkImage image() noexcept { return m_image.image(); }

    VkImageView imageView() noexcept { return m_image.imageView(); }

    VkSampler sampler() noexcept { return m_sampler; }

    TextureType type() const noexcept { return m_type; }

private:
    VkDevice m_device   = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;
    Image m_image;
    TextureType m_type = TextureType::Albedo;
};

} // namespace R3
