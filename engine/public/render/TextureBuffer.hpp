#pragma once

#include <string_view>
#include "render/Buffer.hpp"
#include "render/ImageView.hpp"
#include "render/RenderApi.hpp"
#include "render/Sampler.hpp"
#include "render/Texture.hpp"

namespace R3 {

/// @brief Texture Buffer Specification
/// Only ONE of (data|raw|path) can present, rest must be null
struct R3_API TextureBufferSpecification {
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    const CommandBuffer& commandBuffer;   ///< CommandBuffer
    uint32 width;                         ///< Texture width (ignored if path is null)
    uint32 height;                        ///< Texture height (ignored if path is null)
    const uint8* data;                    ///< Texture compressed data (exclusive)
    const uint8* raw;                     ///< Texture raw data (exclusive)
    const char* path;                     ///< Texture filepath (exclusive)
    TextureType type;                     ///< TextureType
};

/// @brief TextureBuffer allocated from texture data
class R3_API TextureBuffer : public Buffer {
public:
    DEFAULT_CONSTRUCT(TextureBuffer);
    NO_COPY(TextureBuffer);
    DEFAULT_MOVE(TextureBuffer);

    /// @brief Construct TextureBuffer from spec
    /// @param spec
    TextureBuffer(const TextureBufferSpecification& spec);

    /// @brief Free TextureBuffer
    ~TextureBuffer();

    [[nodiscard]] const ImageView& textureView() const { return m_imageView; } ///< Query Texture ImageView
    [[nodiscard]] const Sampler& sampler() const { return m_sampler; }         ///< Query Texture Sampler
    [[nodiscard]] TextureType type() const { return m_type; }                  ///< Query type

private:
    Ref<const LogicalDevice> m_logicalDevice;
    ImageView m_imageView;
    Sampler m_sampler;
    TextureType m_type;
};

} // namespace R3