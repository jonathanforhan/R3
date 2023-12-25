#pragma once

/// @file TextureBuffer.hpp

#include <string_view>
#include "render/Buffer.hpp"
#include "render/ImageView.hpp"
#include "render/RenderApi.hpp"
#include "render/Sampler.hpp"

namespace R3 {

/// @brief Texture Types
enum class TextureType : uint8 {
    Nil = 0,
    Albedo = 1,
    MetallicRoughness = 2,
    Normal = 3,
    AmbientOcclusion = 4,
    Emissive = 5,
};

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

/// @brief Layout of PBR Texture structure
struct PBRTextureResource {
    TextureBuffer::ID albedo = usize(~0);            ///< Albedo id
    TextureBuffer::ID metallicRoughness = usize(~0); ///< MetallicRoughness id
    TextureBuffer::ID normal = usize(~0);            ///< Normal id
    TextureBuffer::ID ambientOcclusion = usize(~0);  ///< AmbientOcclusion id
    TextureBuffer::ID emissive = usize(~0);          ///< Emissive id
};

} // namespace R3