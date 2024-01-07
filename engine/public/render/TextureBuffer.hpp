#pragma once

#include <string_view>
#include "render/Buffer.hpp"
#include "render/ImageView.hpp"
#include "render/RenderApi.hpp"
#include "render/Sampler.hpp"

namespace R3 {

enum class R3_API TextureType : uint8 {
    Nil = 0,
    Albedo = 1,
    MetallicRoughness = 2,
    Normal = 3,
    AmbientOcclusion = 4,
    Emissive = 5,
};

/// @brief Texture Buffer Specification
/// Only ONE of (data|raw|path) can present, rest must be null
struct R3_API TextureBufferSpecification {
    const PhysicalDevice& physicalDevice; ///< PhysicalDevice
    const LogicalDevice& logicalDevice;   ///< LogicalDevice
    const CommandBuffer& commandBuffer;   ///< CommandBuffer
    uint32 width;                         ///< Texture width (ignored if path is null)
    uint32 height;                        ///< Texture height (ignored if path is null)
    const std::byte* data;                ///< Texture compressed data (exclusive)
    const std::byte* raw;                 ///< Texture raw data (exclusive)
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

    [[nodiscard]] constexpr const ImageView& textureView() const { return m_imageView; } ///< Query Texture ImageView
    [[nodiscard]] constexpr const Sampler& sampler() const { return m_sampler; }         ///< Query Texture Sampler
    [[nodiscard]] constexpr TextureType type() const { return m_type; }                  ///< Query type

private:
    Ref<const LogicalDevice> m_logicalDevice;
    ImageView m_imageView;
    Sampler m_sampler;
    TextureType m_type = TextureType::Nil;
};

struct R3_API TexturePBR {
    DEFAULT_CONSTRUCT(TexturePBR);
    NO_COPY(TexturePBR);
    DEFAULT_MOVE(TexturePBR);

    static constexpr uint32 ALBEDO_FLAG = 1 << 0;
    static constexpr uint32 METALLIC_ROUGHNESS_FLAG = 1 << 1;
    static constexpr uint32 NORMAL_FLAG = 1 << 2;
    static constexpr uint32 AMBIENT_OCCLUSION_FLAG = 1 << 3;
    static constexpr uint32 EMISSIVE_FLAG = 1 << 4;

    std::shared_ptr<TextureBuffer> albedo;
    std::shared_ptr<TextureBuffer> metallicRoughness;
    std::shared_ptr<TextureBuffer> normal;
    std::shared_ptr<TextureBuffer> ambientOcclusion;
    std::shared_ptr<TextureBuffer> emissive;
};

} // namespace R3