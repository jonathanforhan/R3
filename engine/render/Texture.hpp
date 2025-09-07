#pragma once

#include <array>
#include <cstddef>
#include <filesystem>
#include "Buffer.hpp"
#include "Flags.hpp"
#include "Image.hpp"
#include "RenderFwd.hpp"
#include "RenderHandle.hpp"
#include "Sampler.hpp"
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

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
    Texture(ICommandBuffer& cmd, const std::byte* raw, usize width, usize height, uint32 channels, TextureType type);

    /// @brief Initializes a texture image and sampler from filesystem using a command buffer and a staging buffer.
    /// @param cmd Reference to the command buffer used for recording GPU commands (must be recording).
    /// @param filepath Path to the texture file to be loaded.
    /// @param type Type of the texture
    Texture(ICommandBuffer& cmd, const std::filesystem::path& filepath, TextureType type);

    /// @brief Initializes a cube-map image and sampler from filesystem using a command buffer and a staging buffer.
    /// @param cmd Reference to the command buffer used for recording GPU commands (must be recording).
    /// @param filepaths Paths to the cubemap texture files to be loaded. Right Left Top Bottom Front Back
    /// @param type Type of the texture
    Texture(ICommandBuffer& cmd, const std::array<std::filesystem::path, 6>& filepaths, TextureType type);

    /// @brief Constructs a Texture object by moving an Image and a Sampler into it.
    /// @param image An rvalue reference to an Image object to be moved into the Texture.
    /// @param sampler An rvalue reference to a Sampler object to be moved into the Texture.
    Texture(Image&& image, Sampler&& sampler) noexcept;

    /// @return A const reference to the underlying image handle.
    const ImageRenderHandle& imageHandle() const noexcept { return m_image.imageHandle(); }
    /// @return A const reference to the underlying image-view handle.
    const ImageViewRenderHandle& imageViewHandle() const noexcept { return m_image.imageViewHandle(); }
    /// @return A const reference to the underlying sampler handle.
    const SamplerRenderHandle& samplerHandle() const noexcept { return m_sampler.samplerHandle(); }
    /// @return A const reference to the internal Image object.
    const Image& image() const noexcept { return m_image; }
    /// @return A const reference to the internal Sampler object.
    const Sampler& sampler() const noexcept { return m_sampler; }

private:
    void create(ICommandBuffer& cmd,
                const std::byte* raw,
                usize width,
                usize height,
                uint32 channels,
                TextureType type);

    static Format queryTextureFormat(TextureType type) noexcept;

    // used for Albedo, Emissive, etc.
    void writeRGBABuffer(const std::byte* raw, usize width, usize height, uint32 channels, Buffer& stagingBuffer);
    void writeMRBuffer(const std::byte* raw, usize width, usize height, uint32 channels, Buffer& stagingBuffer);
    void writeAOBuffer(const std::byte* raw, usize width, usize height, uint32 channels, Buffer& stagingBuffer);
    void writeNormalBuffer(const std::byte* raw, usize width, usize height, uint32 channels, Buffer& stagingBuffer);

private:
    Image m_image;
    Sampler m_sampler;
};

} // namespace R3