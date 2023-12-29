#pragma once

/// @file TextureBuffer.hxx

#include <string_view>
#include "render/Buffer.hxx"
#include "render/ImageView.hxx"
#include "render/RenderApi.hxx"
#include "render/Sampler.hxx"
#include "render/Texture.hpp"

namespace R3 {

/// @brief Texture Buffer Specification
struct TextureBufferSpecification {
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    const Swapchain& swapchain;           ///< Swapchain
    const CommandBuffer& commandBuffer;   ///< CommandBuffer
    uint32 width;                         ///< Texture width (ignored if path is null)
    uint32 height;                        ///< Texture height (ignored if path is null)
    const uint8* data;                    ///< Texture compressed data (exclusive)
    const uint8* raw;                     ///< Texture raw data (exclusive)
    const char* path;                     ///< Texture filepath (exclusive)
    TextureType type;                     ///< TextureType
};

/// @brief TextureBuffer allocated from texture data
class TextureBuffer : public Buffer {
public:
    DEFAULT_CONSTRUCT(TextureBuffer);
    NO_COPY(TextureBuffer);
    DEFAULT_MOVE(TextureBuffer);

    /// @brief Construct TextureBuffer from spec
    /// @param spec
    TextureBuffer(const TextureBufferSpecification& spec);

    /// @brief Free TextureBuffer
    ~TextureBuffer();

    TextureType type() const { return m_type; }                  ///< Query type
    const ImageView& textureView() const { return m_imageView; } ///< Query Texture ImageView
    const Sampler& sampler() const { return m_sampler; }         ///< Query Texture Sampler

private:
    Ref<const LogicalDevice> m_logicalDevice;
    TextureType m_type;
    ImageView m_imageView;
    Sampler m_sampler;
};

} // namespace R3