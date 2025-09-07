#include "Texture.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <memory>
#include <utility>
#include <vector>
#include "Buffer.hpp"
#include "CommandBuffer.hpp"
#include "Flags.hpp"
#include "Image.hpp"
#include "Sampler.hpp"
#include "api/Assert.hpp"
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "core/ResourceManager.hpp"
#include "media/ImageLoader.hpp"

namespace R3 {

Texture::Texture(ICommandBuffer& cmd,
                 const std::byte* raw,
                 usize width,
                 usize height,
                 uint32 channels,
                 TextureType type) {
    create(cmd, raw, width, height, channels, type);
}

Texture::Texture(ICommandBuffer& cmd, const std::filesystem::path& filepath, TextureType type) {
    auto imgDesc = ImageLoader().loadImageFile(filepath);
    create(cmd, imgDesc.data.get(), imgDesc.width, imgDesc.height, imgDesc.channels, type);
}

Texture::Texture(ICommandBuffer& cmd, const std::array<std::filesystem::path, 6>& filepaths, TextureType type) {
    R3_ASSERT(type == TextureType::Cubemap);

    std::vector<std::byte> raw;
    usize width{}, height{};
    uint32 channels{};

    for (auto& path : filepaths) {
        const auto imgDesc  = ImageLoader().loadImageFile(path, 4);
        width               = imgDesc.width;
        height              = imgDesc.height;
        channels            = imgDesc.channels;
        const usize imgSize = width * height * channels;
        const usize rawSize = raw.size();

        raw.resize(raw.size() + imgSize);
        std::copy(imgDesc.data.get(), imgDesc.data.get() + imgSize, raw.begin() + rawSize);
    }

    create(cmd, raw.data(), width, height, channels, type);
}

Texture::Texture(Image&& image, Sampler&& sampler) noexcept
    : m_image(std::move(image)),
      m_sampler(std::move(sampler)) {}

void Texture::create(ICommandBuffer& cmd,
                     const std::byte* raw,
                     usize width,
                     usize height,
                     uint32 channels,
                     TextureType type) {
    bool isCube = type == TextureType::DepthCubemap || type == TextureType::Cubemap;

    const Format textureFormat    = queryTextureFormat(type);
    const uint32 texturePixelSize = formatPixelSize(textureFormat);
    const uint32 mipLevels        = static_cast<uint32>(std::floor(std::log2(std::max(width, height)))) + 1;
    const ImageType imageType     = isCube ? ImageType::ImageCube : ImageType::Image2D;
    ImageUsageFlags imageUsage    = ImageUsage::Texture;
    AddressMode addressMode       = AddressMode::Repeat;
    const uint32 imgSize          = (uint32)(width * height) * texturePixelSize * (isCube ? 6U : 1U);

    Buffer* stagingBuffer = GResourceManager()->newFrameScopedObject<Buffer>(imgSize, BufferUsage::HostStaging);

    switch (type) {
        case TextureType::MetallicRoughness:
            writeMRBuffer(raw, width, height, channels, *stagingBuffer);
            break;
        case TextureType::Normal:
            writeNormalBuffer(raw, width, height, channels, *stagingBuffer);
            break;
        case TextureType::AmbientOcclusion:
            writeAOBuffer(raw, width, height, channels, *stagingBuffer);
            break;
        case TextureType::Albedo:
        case TextureType::Emissive:
            writeRGBABuffer(raw, width, height, channels, *stagingBuffer);
            break;
        case TextureType::Cubemap:
            R3_ASSERT(channels == 4, "Cubemap require 4 channels on creation");
            addressMode = AddressMode::ClampToEdge;
            for (usize i = 0; i < 6; i++) {
                const usize size       = width * height * channels;
                const usize faceOffset = i * size;
                stagingBuffer->copy(&raw[faceOffset], faceOffset, size);
            }
            break;
        case TextureType::Depth:
        case TextureType::DepthCubemap:
            addressMode = AddressMode::ClampToEdge;
            imageUsage  = ImageUsage::DepthStencilAttachment;
            break;
        default:
            R3_ASSERT(false, "Invalid TextureType");
            break;
    }

    m_image = Image{{width, height, 1}, mipLevels, 1, imageUsage, textureFormat, imageType};

    if (imageUsage == ImageUsage::Texture) {
        cmd.copyBufferToImage(*stagingBuffer, m_image);
        m_image.generateMipmaps(cmd);
    }

    m_sampler = Sampler{Filter::Linear, Filter::Linear, MipmapMode::Linear, addressMode};
}

Format Texture::queryTextureFormat(TextureType type) noexcept {
    switch (type) {
        case TextureType::Albedo:
            return Format::R8G8B8A8_SRGB;
        case TextureType::MetallicRoughness:
            return Format::R8G8_UNORM;
        case TextureType::Normal:
            return Format::R8G8_UNORM;
        case TextureType::AmbientOcclusion:
            return Format::R8_UNORM;
        case TextureType::Emissive:
            return Format::R8G8B8A8_UNORM;
        case TextureType::Cubemap:
            return Format::R8G8B8A8_SRGB;
        case TextureType::Depth:
        case TextureType::DepthCubemap:
            return Format::D32_SFLOAT;
        default:
            R3_ASSERT(false, "Invalid PBR TextureType");
            return Format::Undefined;
    }
}

void Texture::writeRGBABuffer(const std::byte* raw, usize width, usize height, uint32 channels, Buffer& stagingBuffer) {
    const usize imgSize = width * height * 4;
    const usize rawSize = width * height * channels;

    if (channels == 4) {
        R3_ASSERT(imgSize == rawSize);
        stagingBuffer.copy(raw, 0, rawSize);
    } else {
        uint8 color[4] = {0, 0, 0, 255};
        for (usize rgba = 0, ch = 0; rgba < imgSize; rgba += 4, ch += channels) {
            R3_ASSERT(ch < rawSize);
            std::memcpy(color, &raw[ch], channels);
            stagingBuffer.copy(color, rgba, 4);
        }
    }
}

void Texture::writeMRBuffer(const std::byte* raw, usize width, usize height, uint32 channels, Buffer& stagingBuffer) {
    const usize imgSize = width * height * 2;
    const usize rawSize = width * height * channels;

    // copy the GB channels from raw to staging buffer's RG channels
    for (usize rg = 0, ch = 0; rg < imgSize; rg += 2, ch += channels) {
        R3_ASSERT(ch < rawSize);
        stagingBuffer.copy(&raw[ch + 1], rg, 2);
    }
}

void Texture::writeAOBuffer(const std::byte* raw, usize width, usize height, uint32 channels, Buffer& stagingBuffer) {
    const usize imgSize = width * height * 1;
    const usize rawSize = width * height * channels;

    // copy the R channel from raw to staging buffer's R channel
    for (usize r = 0, ch = 0; r < imgSize; r += 1, ch += channels) {
        R3_ASSERT(ch < rawSize);
        stagingBuffer.copy(&raw[ch], r, 1);
    }
}

void Texture::writeNormalBuffer(const std::byte* raw,
                                usize width,
                                usize height,
                                uint32 channels,
                                Buffer& stagingBuffer) {
    const usize imgSize = width * height * 2;
    const usize rawSize = width * height * channels;

    // copy the RG channels from raw to staging buffer's RG channels
    for (usize rg = 0, ch = 0; rg < imgSize; rg += 2, ch += channels) {
        R3_ASSERT(ch < rawSize);
        stagingBuffer.copy(&raw[ch], rg, 2);
    }
}

} // namespace R3
