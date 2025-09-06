#pragma once

#include "engine/api/Api.hpp"

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

/// @brief Texture class R3_API encapsulates a Vulkan texture image and sampler.
/// It provides multiple construct R3_APIors to create textures from raw data, compressed data, or from a file path.
/// It initializes a temporary staging buffer that must outlive the command buffer commands used for texture upload.
/// Also initializes an image object passed that must outlive the texture object, as the texture refernces it.
class R3_API Texture {
public:
    R3_CTOR_DEFAULT(Texture);
    R3_COPY_DELETE(Texture);
    R3_MOVE_DEFAULT(Texture);

    /// @brief Initializes a texture image and sampler from raw data using a command buffer and a staging buffer.
    /// @param cmd Reference to the command buffer used for recording GPU commands (must be recording).
    /// @param raw Pointer to the raw texture data.
    /// @param width Width of the texture in bytes.
    /// @param height Height of the texture in bytes.
    /// @param channels Channels of the texture in bytes.
    /// @param type Type of the texture
    /// @param stagingBuffer Buffer used for staging the texture data before transfer (must live until cmd.submit()).
    Texture(CommandBuffer& cmd,
            const std::byte* raw,
            usize width,
            usize height,
            uint32 channels,
            TextureType type,
            Buffer& stagingBuffer);

    /// @brief Initializes a texture image and sampler from filesystem using a command buffer and a staging buffer.
    /// @param cmd Reference to the command buffer used for recording GPU commands (must be recording).
    /// @param filepath Path to the texture file to be loaded.
    /// @param type Type of the texture
    /// @param stagingBuffer Buffer used for staging the texture data before transfer (must live until cmd.submit()).
    Texture(CommandBuffer& cmd, const std::filesystem::path& filepath, TextureType type, Buffer& stagingBuffer);

    ~Texture() noexcept;

    VkSampler sampler() const noexcept { return m_sampler; }

    VkImage image() const noexcept { return m_image.imageHandle(); }

    VkImageView imageView() const noexcept { return m_image.imageViewHandle(); }

private:
    void create(CommandBuffer& cmd,
                const std::byte* raw,
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
