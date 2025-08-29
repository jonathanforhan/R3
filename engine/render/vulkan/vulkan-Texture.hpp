#pragma once

#include <array>
#include <cstddef>
#include <filesystem>
#include <volk.h>
#include "api/Class.hpp"
#include "api/Types.hpp"
#include "render/Flags.hpp"
#include "vulkan-Fwd.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-Image.hpp"

namespace R3::vulkan {

/// @brief Texture class encapsulates a Vulkan texture image and sampler.
/// It provides multiple constructors to create textures from raw data, compressed data, or from a file path.
/// It initializes a temporary staging buffer that must outlive the command buffer commands used for texture upload.
/// Also initializes an image object passed that must outlive the texture object, as the texture refernces it.
class Texture {
public:
    R3_CTOR_DEFAULT(Texture);
    R3_COPY_DELETE(Texture);
    R3_MOVE_DEFAULT(Texture);

    /// @brief Initializes a texture image and sampler from raw data using a command buffer and a staging buffer.
    /// @param cmd Reference to the command buffer used for recording GPU commands (must be recording).
    /// @param raw Pointer to the raw texture data.
    /// @param width Width of the texture in pixels.
    /// @param height Height of the texture in pixels.
    /// @param type Type of the texture
    /// @param stagingBuffer Buffer used for staging the texture data before transfer (must live until cmd.submit()).
    Texture(CommandBuffer& cmd,
            const std::byte* raw,
            usize width,
            usize height,
            TextureType type,
            Buffer& stagingBuffer);

    /// @brief Initializes a texture image and sampler from compressed data using a command buffer and a staging buffer.
    /// @param cmd Reference to the command buffer used for recording GPU commands (must be recording).
    /// @param compressed Pointer to the compressed texture data.
    /// @param size The size, in bytes, of the compressed data.
    /// @param type Type of the texture
    /// @param stagingBuffer Buffer used for staging the texture data before transfer (must live until cmd.submit()).
    Texture(CommandBuffer& cmd, const std::byte* compressed, usize size, TextureType type, Buffer& stagingBuffer);

    /// @brief Initializes a texture image and sampler from filesystem using a command buffer and a staging buffer.
    /// @param cmd Reference to the command buffer used for recording GPU commands (must be recording).
    /// @param filepath Path to the texture file to be loaded.
    /// @param type Type of the texture
    /// @param stagingBuffer Buffer used for staging the texture data before transfer (must live until cmd.submit()).
    Texture(CommandBuffer& cmd, const std::filesystem::path& filepath, TextureType type, Buffer& stagingBuffer);

    /// @brief Create cube texture map from 6 file paths
    /// @param cmd Command buffer for recording GPU commands
    /// @param facePaths Array of 6 file paths in order: +X, -X, +Y, -Y, +Z, -Z
    /// @param type Texture type for format selection
    /// @param stagingBuffer Staging buffer for transfer
    Texture(CommandBuffer& cmd,
            const std::array<std::filesystem::path, 6>& facePaths,
            TextureType type,
            Buffer& stagingBuffer);

    ~Texture() noexcept;

    VkSampler sampler() const noexcept { return m_sampler; }

    VkImageView imageView() const noexcept { return m_image.imageView(); }

private:
    void create(CommandBuffer& cmd,
                const std::byte* raw,
                usize width,
                usize height,
                TextureType type,
                Buffer& stagingBuffer);

    void createCubeMap(CommandBuffer& cmd,
                       std::array<const std::byte*, 6> faces,
                       usize width,
                       usize height,
                       TextureType type,
                       Buffer& stagingBuffer);

    bool supportsBlitting(VkFormat format) noexcept;

    VkFormat queryPreferredFormat(TextureType type) const noexcept;

private:
    Handle<VkSampler> m_sampler;
    Image m_image;
};

} // namespace R3::vulkan
