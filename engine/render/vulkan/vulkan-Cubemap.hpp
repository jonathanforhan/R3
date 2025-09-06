#pragma once

#include "engine/api/Api.hpp"

#include <array>
#include <cstddef>
#include <filesystem>
#include <vulkan/vulkan.h>
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "engine/render/Buffer.hpp"
#include "engine/render/Flags.hpp"
#include "engine/render/Image.hpp"
#include "vulkan-Fwd.hpp"
#include "vulkan-Handle.hpp"

namespace R3::vulkan {

class R3_API Cubemap {
public:
    R3_CTOR_DEFAULT(Cubemap);
    R3_COPY_DELETE(Cubemap);
    R3_MOVE_DEFAULT(Cubemap);

    Cubemap(CommandBuffer& cmd,
            const std::array<const std::byte*, 6>& raw,
            usize width,
            usize height,
            uint32 channels,
            TextureType type,
            Buffer& stagingBuffer);

    Cubemap(CommandBuffer& cmd,
            const std::array<std::filesystem::path, 6>& facePaths,
            TextureType type,
            Buffer& stagingBuffer);

    ~Cubemap() noexcept;

    VkSampler sampler() const noexcept { return m_sampler; }

    VkImage image() const noexcept { return m_image.imageHandle(); }

    VkImageView imageView() const noexcept { return m_image.imageViewHandle(); }

private:
    void create(CommandBuffer& cmd,
                std::array<const std::byte*, 6> faces,
                usize width,
                usize height,
                uint32 channels,
                TextureType type,
                Buffer& stagingBuffer);

    bool supportsBlitting(VkFormat format) noexcept;

    VkFormat queryPreferredFormat(TextureType type) const noexcept;

    uint32 queryPreferredChannels(TextureType type) const noexcept;

private:
    Handle<VkSampler> m_sampler;
    Image m_image;
};

} // namespace R3::vulkan
