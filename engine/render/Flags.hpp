#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

enum class R3_API QueueType {
    Present,
    Graphics,
    Compute,
    Transfer,
};

/// @brief Defines usage flags for buffer resources, specifying their intended operations and memory properties.
struct R3_API BufferUsage {
    enum : flag_t {
        TransferSrc     = 0x0000'0001,
        TransferDst     = 0x0000'0002,
        UniformTexel    = 0x0000'0004,
        StorageTexel    = 0x0000'0008,
        Uniform         = 0x0000'0010,
        Storage         = 0x0000'0020,
        Index           = 0x0000'0040,
        Vertex          = 0x0000'0080,
        DeviceLocal     = 0x0001'0000,
        HostVisible     = 0x0002'0000,
        HostCoherent    = 0x0004'0000,
        LazilyAllocated = 0x0010'0000,
        MapOnCreation   = 0x8000'0000,
        HostStaging     = TransferSrc | HostVisible | HostCoherent | MapOnCreation,
        HostUniform     = Uniform | HostVisible | HostCoherent | MapOnCreation,
        HostStorage     = Storage | HostVisible | HostCoherent | MapOnCreation,
        DeviceUniform   = TransferDst | Uniform | DeviceLocal,
        DeviceStorage   = TransferDst | Storage | DeviceLocal,
        DeviceIndex     = TransferDst | Index | DeviceLocal,
        DeviceVertex    = TransferDst | Vertex | DeviceLocal,
    };
};
using BufferUsageFlags = flag_t;

/// @brief Defines the type of image resource, specifying its dimensionality and array properties.
enum class R3_API ImageType {
    Image1D   = 0,
    Image2D   = 1,
    Image3D   = 2,
    ImageCube = 3,
    // Image1DArray   = 4,
    // Image2DArray   = 5,
    // ImageCubeArray = 6,
};

/// @brief Defines usage flags for image resources, specifying their intended operations and memory properties.
/// @note every usage implicitly includes sampling capability
struct R3_API ImageUsage {
    enum : flag_t {
        TransferSrc            = 0x0000'0001,
        TransferDst            = 0x0000'0002,
        Storage                = 0x0000'0008,
        ColorAttachment        = 0x0000'0010,
        DepthStencilAttachment = 0x0000'0020,
        TransientAttachment    = 0x0000'0040,
        InputAttachment        = 0x0000'0080,
        Texture                = TransferSrc | TransferDst,
    };
};
using ImageUsageFlags = flag_t;

/// @brief Defines various pixel formats for image resources, specifying the data layout and type.
enum class R3_API Format {
    Undefined           = 0,
    R8_UNORM            = 9,
    R8G8_UNORM          = 16,
    R8G8B8A8_UNORM      = 37,
    R8G8B8A8_SRGB       = 43,
    B8G8R8A8_SRGB       = 50,
    R16_SFLOAT          = 76,
    R16G16_SFLOAT       = 83,
    R16G16B16A16_SFLOAT = 97,
    R32_SFLOAT          = 100,
    R32G32_SFLOAT       = 103,
    R32G32B32_SFLOAT    = 106,
    R32G32B32A32_SFLOAT = 109,
    D16_UNORM           = 124,
    D24_UNORM           = 125,
    D32_SFLOAT          = 126,
    D24_UNORM_S8_UINT   = 129,
};

constexpr uint32 R3_API formatPixelSize(Format format) {
    switch (format) {
        case Format::R8_UNORM:
            return 1;
        case Format::R8G8_UNORM:
            return 2;
        case Format::R8G8B8A8_UNORM:
            return 4;
        case Format::R8G8B8A8_SRGB:
            return 4;
        case Format::B8G8R8A8_SRGB:
            return 4;
        case Format::R16_SFLOAT:
            return 2;
        case Format::R16G16_SFLOAT:
            return 4;
        case Format::R16G16B16A16_SFLOAT:
            return 8;
        case Format::R32_SFLOAT:
            return 4;
        case Format::R32G32_SFLOAT:
            return 8;
        case Format::R32G32B32_SFLOAT:
            return 12;
        case Format::R32G32B32A32_SFLOAT:
            return 16;
        case Format::D16_UNORM:
            return 2;
        case Format::D24_UNORM:
            return 3;
        case Format::D32_SFLOAT:
            return 4;
        case Format::D24_UNORM_S8_UINT:
            return 4;
        default:
            return 0;
    }
}

/// @brief Enumerates the available filtering modes.
enum class R3_API Filter {
    Nearest = 0,
    Linear  = 1,
};

/// @brief Enumerates the available mipmap filtering modes.
enum class R3_API MipmapMode {
    Nearest = 0,
    Linear  = 1,
};

/// @brief Enumerates the possible modes for addressing texture coordinates.
enum class R3_API AddressMode {
    Repeat         = 0,
    MirroredRepeat = 1,
    ClampToEdge    = 2,
    ClampToBorder  = 3,
};

/// @brief Defines different types of textures used in rendering, specifying their intended use cases.
enum class R3_API TextureType {
    Albedo            = 0,
    MetallicRoughness = 1,
    Normal            = 2,
    AmbientOcclusion  = 3,
    Emissive          = 4,
    Cubemap           = 5,
    Depth             = 6,
    DepthCubemap      = 7,
};

} // namespace R3
