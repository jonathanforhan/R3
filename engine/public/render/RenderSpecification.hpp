#pragma once

/// @file RenderSpecification.hpp
/// @brief Copied from vulkan_core.h
/// All values are Vulkan Native, convert for capability with other renderers

namespace R3 {

constexpr auto MAX_FRAMES_IN_FLIGHT = 3;

enum Format {
    R3_FORMAT_UNDEFINED = 0,
    R3_FORMAT_R4G4_UNORM_PACK8 = 1,
    R3_FORMAT_R4G4B4A4_UNORM_PACK16 = 2,
    R3_FORMAT_B4G4R4A4_UNORM_PACK16 = 3,
    R3_FORMAT_R5G6B5_UNORM_PACK16 = 4,
    R3_FORMAT_B5G6R5_UNORM_PACK16 = 5,
    R3_FORMAT_R5G5B5A1_UNORM_PACK16 = 6,
    R3_FORMAT_B5G5R5A1_UNORM_PACK16 = 7,
    R3_FORMAT_A1R5G5B5_UNORM_PACK16 = 8,
    R3_FORMAT_R8_UNORM = 9,
    R3_FORMAT_R8_SNORM = 10,
    R3_FORMAT_R8_USCALED = 11,
    R3_FORMAT_R8_SSCALED = 12,
    R3_FORMAT_R8_UINT = 13,
    R3_FORMAT_R8_SINT = 14,
    R3_FORMAT_R8_SRGB = 15,
    R3_FORMAT_R8G8_UNORM = 16,
    R3_FORMAT_R8G8_SNORM = 17,
    R3_FORMAT_R8G8_USCALED = 18,
    R3_FORMAT_R8G8_SSCALED = 19,
    R3_FORMAT_R8G8_UINT = 20,
    R3_FORMAT_R8G8_SINT = 21,
    R3_FORMAT_R8G8_SRGB = 22,
    R3_FORMAT_R8G8B8_UNORM = 23,
    R3_FORMAT_R8G8B8_SNORM = 24,
    R3_FORMAT_R8G8B8_USCALED = 25,
    R3_FORMAT_R8G8B8_SSCALED = 26,
    R3_FORMAT_R8G8B8_UINT = 27,
    R3_FORMAT_R8G8B8_SINT = 28,
    R3_FORMAT_R8G8B8_SRGB = 29,
    R3_FORMAT_B8G8R8_UNORM = 30,
    R3_FORMAT_B8G8R8_SNORM = 31,
    R3_FORMAT_B8G8R8_USCALED = 32,
    R3_FORMAT_B8G8R8_SSCALED = 33,
    R3_FORMAT_B8G8R8_UINT = 34,
    R3_FORMAT_B8G8R8_SINT = 35,
    R3_FORMAT_B8G8R8_SRGB = 36,
    R3_FORMAT_R8G8B8A8_UNORM = 37,
    R3_FORMAT_R8G8B8A8_SNORM = 38,
    R3_FORMAT_R8G8B8A8_USCALED = 39,
    R3_FORMAT_R8G8B8A8_SSCALED = 40,
    R3_FORMAT_R8G8B8A8_UINT = 41,
    R3_FORMAT_R8G8B8A8_SINT = 42,
    R3_FORMAT_R8G8B8A8_SRGB = 43,
    R3_FORMAT_B8G8R8A8_UNORM = 44,
    R3_FORMAT_B8G8R8A8_SNORM = 45,
    R3_FORMAT_B8G8R8A8_USCALED = 46,
    R3_FORMAT_B8G8R8A8_SSCALED = 47,
    R3_FORMAT_B8G8R8A8_UINT = 48,
    R3_FORMAT_B8G8R8A8_SINT = 49,
    R3_FORMAT_B8G8R8A8_SRGB = 50,
    R3_FORMAT_A8B8G8R8_UNORM_PACK32 = 51,
    R3_FORMAT_A8B8G8R8_SNORM_PACK32 = 52,
    R3_FORMAT_A8B8G8R8_USCALED_PACK32 = 53,
    R3_FORMAT_A8B8G8R8_SSCALED_PACK32 = 54,
    R3_FORMAT_A8B8G8R8_UINT_PACK32 = 55,
    R3_FORMAT_A8B8G8R8_SINT_PACK32 = 56,
    R3_FORMAT_A8B8G8R8_SRGB_PACK32 = 57,
    R3_FORMAT_A2R10G10B10_UNORM_PACK32 = 58,
    R3_FORMAT_A2R10G10B10_SNORM_PACK32 = 59,
    R3_FORMAT_A2R10G10B10_USCALED_PACK32 = 60,
    R3_FORMAT_A2R10G10B10_SSCALED_PACK32 = 61,
    R3_FORMAT_A2R10G10B10_UINT_PACK32 = 62,
    R3_FORMAT_A2R10G10B10_SINT_PACK32 = 63,
    R3_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,
    R3_FORMAT_A2B10G10R10_SNORM_PACK32 = 65,
    R3_FORMAT_A2B10G10R10_USCALED_PACK32 = 66,
    R3_FORMAT_A2B10G10R10_SSCALED_PACK32 = 67,
    R3_FORMAT_A2B10G10R10_UINT_PACK32 = 68,
    R3_FORMAT_A2B10G10R10_SINT_PACK32 = 69,
    R3_FORMAT_R16_UNORM = 70,
    R3_FORMAT_R16_SNORM = 71,
    R3_FORMAT_R16_USCALED = 72,
    R3_FORMAT_R16_SSCALED = 73,
    R3_FORMAT_R16_UINT = 74,
    R3_FORMAT_R16_SINT = 75,
    R3_FORMAT_R16_SFLOAT = 76,
    R3_FORMAT_R16G16_UNORM = 77,
    R3_FORMAT_R16G16_SNORM = 78,
    R3_FORMAT_R16G16_USCALED = 79,
    R3_FORMAT_R16G16_SSCALED = 80,
    R3_FORMAT_R16G16_UINT = 81,
    R3_FORMAT_R16G16_SINT = 82,
    R3_FORMAT_R16G16_SFLOAT = 83,
    R3_FORMAT_R16G16B16_UNORM = 84,
    R3_FORMAT_R16G16B16_SNORM = 85,
    R3_FORMAT_R16G16B16_USCALED = 86,
    R3_FORMAT_R16G16B16_SSCALED = 87,
    R3_FORMAT_R16G16B16_UINT = 88,
    R3_FORMAT_R16G16B16_SINT = 89,
    R3_FORMAT_R16G16B16_SFLOAT = 90,
    R3_FORMAT_R16G16B16A16_UNORM = 91,
    R3_FORMAT_R16G16B16A16_SNORM = 92,
    R3_FORMAT_R16G16B16A16_USCALED = 93,
    R3_FORMAT_R16G16B16A16_SSCALED = 94,
    R3_FORMAT_R16G16B16A16_UINT = 95,
    R3_FORMAT_R16G16B16A16_SINT = 96,
    R3_FORMAT_R16G16B16A16_SFLOAT = 97,
    R3_FORMAT_R32_UINT = 98,
    R3_FORMAT_R32_SINT = 99,
    R3_FORMAT_R32_SFLOAT = 100,
    R3_FORMAT_R32G32_UINT = 101,
    R3_FORMAT_R32G32_SINT = 102,
    R3_FORMAT_R32G32_SFLOAT = 103,
    R3_FORMAT_R32G32B32_UINT = 104,
    R3_FORMAT_R32G32B32_SINT = 105,
    R3_FORMAT_R32G32B32_SFLOAT = 106,
    R3_FORMAT_R32G32B32A32_UINT = 107,
    R3_FORMAT_R32G32B32A32_SINT = 108,
    R3_FORMAT_R32G32B32A32_SFLOAT = 109,
    R3_FORMAT_R64_UINT = 110,
    R3_FORMAT_R64_SINT = 111,
    R3_FORMAT_R64_SFLOAT = 112,
    R3_FORMAT_R64G64_UINT = 113,
    R3_FORMAT_R64G64_SINT = 114,
    R3_FORMAT_R64G64_SFLOAT = 115,
    R3_FORMAT_R64G64B64_UINT = 116,
    R3_FORMAT_R64G64B64_SINT = 117,
    R3_FORMAT_R64G64B64_SFLOAT = 118,
    R3_FORMAT_R64G64B64A64_UINT = 119,
    R3_FORMAT_R64G64B64A64_SINT = 120,
    R3_FORMAT_R64G64B64A64_SFLOAT = 121,
    R3_FORMAT_B10G11R11_UFLOAT_PACK32 = 122,
    R3_FORMAT_E5B9G9R9_UFLOAT_PACK32 = 123,
    R3_FORMAT_D16_UNORM = 124,
    R3_FORMAT_X8_D24_UNORM_PACK32 = 125,
    R3_FORMAT_D32_SFLOAT = 126,
    R3_FORMAT_S8_UINT = 127,
    R3_FORMAT_D16_UNORM_S8_UINT = 128,
    R3_FORMAT_D24_UNORM_S8_UINT = 129,
    R3_FORMAT_D32_SFLOAT_S8_UINT = 130,
    R3_FORMAT_BC1_RGB_UNORM_BLOCK = 131,
    R3_FORMAT_BC1_RGB_SRGB_BLOCK = 132,
    R3_FORMAT_BC1_RGBA_UNORM_BLOCK = 133,
    R3_FORMAT_BC1_RGBA_SRGB_BLOCK = 134,
    R3_FORMAT_BC2_UNORM_BLOCK = 135,
    R3_FORMAT_BC2_SRGB_BLOCK = 136,
    R3_FORMAT_BC3_UNORM_BLOCK = 137,
    R3_FORMAT_BC3_SRGB_BLOCK = 138,
    R3_FORMAT_BC4_UNORM_BLOCK = 139,
    R3_FORMAT_BC4_SNORM_BLOCK = 140,
    R3_FORMAT_BC5_UNORM_BLOCK = 141,
    R3_FORMAT_BC5_SNORM_BLOCK = 142,
    R3_FORMAT_BC6H_UFLOAT_BLOCK = 143,
    R3_FORMAT_BC6H_SFLOAT_BLOCK = 144,
    R3_FORMAT_BC7_UNORM_BLOCK = 145,
    R3_FORMAT_BC7_SRGB_BLOCK = 146,
    R3_FORMAT_ETC2_R8G8B8_UNORM_BLOCK = 147,
    R3_FORMAT_ETC2_R8G8B8_SRGB_BLOCK = 148,
    R3_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK = 149,
    R3_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK = 150,
    R3_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK = 151,
    R3_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK = 152,
    R3_FORMAT_EAC_R11_UNORM_BLOCK = 153,
    R3_FORMAT_EAC_R11_SNORM_BLOCK = 154,
    R3_FORMAT_EAC_R11G11_UNORM_BLOCK = 155,
    R3_FORMAT_EAC_R11G11_SNORM_BLOCK = 156,
    R3_FORMAT_ASTC_4x4_UNORM_BLOCK = 157,
    R3_FORMAT_ASTC_4x4_SRGB_BLOCK = 158,
    R3_FORMAT_ASTC_5x4_UNORM_BLOCK = 159,
    R3_FORMAT_ASTC_5x4_SRGB_BLOCK = 160,
    R3_FORMAT_ASTC_5x5_UNORM_BLOCK = 161,
    R3_FORMAT_ASTC_5x5_SRGB_BLOCK = 162,
    R3_FORMAT_ASTC_6x5_UNORM_BLOCK = 163,
    R3_FORMAT_ASTC_6x5_SRGB_BLOCK = 164,
    R3_FORMAT_ASTC_6x6_UNORM_BLOCK = 165,
    R3_FORMAT_ASTC_6x6_SRGB_BLOCK = 166,
    R3_FORMAT_ASTC_8x5_UNORM_BLOCK = 167,
    R3_FORMAT_ASTC_8x5_SRGB_BLOCK = 168,
    R3_FORMAT_ASTC_8x6_UNORM_BLOCK = 169,
    R3_FORMAT_ASTC_8x6_SRGB_BLOCK = 170,
    R3_FORMAT_ASTC_8x8_UNORM_BLOCK = 171,
    R3_FORMAT_ASTC_8x8_SRGB_BLOCK = 172,
    R3_FORMAT_ASTC_10x5_UNORM_BLOCK = 173,
    R3_FORMAT_ASTC_10x5_SRGB_BLOCK = 174,
    R3_FORMAT_ASTC_10x6_UNORM_BLOCK = 175,
    R3_FORMAT_ASTC_10x6_SRGB_BLOCK = 176,
    R3_FORMAT_ASTC_10x8_UNORM_BLOCK = 177,
    R3_FORMAT_ASTC_10x8_SRGB_BLOCK = 178,
    R3_FORMAT_ASTC_10x10_UNORM_BLOCK = 179,
    R3_FORMAT_ASTC_10x10_SRGB_BLOCK = 180,
    R3_FORMAT_ASTC_12x10_UNORM_BLOCK = 181,
    R3_FORMAT_ASTC_12x10_SRGB_BLOCK = 182,
    R3_FORMAT_ASTC_12x12_UNORM_BLOCK = 183,
    R3_FORMAT_ASTC_12x12_SRGB_BLOCK = 184,
    R3_FORMAT_G8B8G8R8_422_UNORM = 1000156000,
    R3_FORMAT_B8G8R8G8_422_UNORM = 1000156001,
    R3_FORMAT_G8_B8_R8_3PLANE_420_UNORM = 1000156002,
    R3_FORMAT_G8_B8R8_2PLANE_420_UNORM = 1000156003,
    R3_FORMAT_G8_B8_R8_3PLANE_422_UNORM = 1000156004,
    R3_FORMAT_G8_B8R8_2PLANE_422_UNORM = 1000156005,
    R3_FORMAT_G8_B8_R8_3PLANE_444_UNORM = 1000156006,
    R3_FORMAT_R10X6_UNORM_PACK16 = 1000156007,
    R3_FORMAT_R10X6G10X6_UNORM_2PACK16 = 1000156008,
    R3_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 1000156009,
    R3_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 1000156010,
    R3_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 1000156011,
    R3_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 1000156012,
    R3_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 1000156013,
    R3_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 1000156014,
    R3_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 1000156015,
    R3_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 1000156016,
    R3_FORMAT_R12X4_UNORM_PACK16 = 1000156017,
    R3_FORMAT_R12X4G12X4_UNORM_2PACK16 = 1000156018,
    R3_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 1000156019,
    R3_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 1000156020,
    R3_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 1000156021,
    R3_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 1000156022,
    R3_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 1000156023,
    R3_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 1000156024,
    R3_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 1000156025,
    R3_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 1000156026,
    R3_FORMAT_G16B16G16R16_422_UNORM = 1000156027,
    R3_FORMAT_B16G16R16G16_422_UNORM = 1000156028,
    R3_FORMAT_G16_B16_R16_3PLANE_420_UNORM = 1000156029,
    R3_FORMAT_G16_B16R16_2PLANE_420_UNORM = 1000156030,
    R3_FORMAT_G16_B16_R16_3PLANE_422_UNORM = 1000156031,
    R3_FORMAT_G16_B16R16_2PLANE_422_UNORM = 1000156032,
    R3_FORMAT_G16_B16_R16_3PLANE_444_UNORM = 1000156033,
    R3_FORMAT_G8_B8R8_2PLANE_444_UNORM = 1000330000,
    R3_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16 = 1000330001,
    R3_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16 = 1000330002,
    R3_FORMAT_G16_B16R16_2PLANE_444_UNORM = 1000330003,
    R3_FORMAT_A4R4G4B4_UNORM_PACK16 = 1000340000,
    R3_FORMAT_A4B4G4R4_UNORM_PACK16 = 1000340001,
    R3_FORMAT_ASTC_4x4_SFLOAT_BLOCK = 1000066000,
    R3_FORMAT_ASTC_5x4_SFLOAT_BLOCK = 1000066001,
    R3_FORMAT_ASTC_5x5_SFLOAT_BLOCK = 1000066002,
    R3_FORMAT_ASTC_6x5_SFLOAT_BLOCK = 1000066003,
    R3_FORMAT_ASTC_6x6_SFLOAT_BLOCK = 1000066004,
    R3_FORMAT_ASTC_8x5_SFLOAT_BLOCK = 1000066005,
    R3_FORMAT_ASTC_8x6_SFLOAT_BLOCK = 1000066006,
    R3_FORMAT_ASTC_8x8_SFLOAT_BLOCK = 1000066007,
    R3_FORMAT_ASTC_10x5_SFLOAT_BLOCK = 1000066008,
    R3_FORMAT_ASTC_10x6_SFLOAT_BLOCK = 1000066009,
    R3_FORMAT_ASTC_10x8_SFLOAT_BLOCK = 1000066010,
    R3_FORMAT_ASTC_10x10_SFLOAT_BLOCK = 1000066011,
    R3_FORMAT_ASTC_12x10_SFLOAT_BLOCK = 1000066012,
    R3_FORMAT_ASTC_12x12_SFLOAT_BLOCK = 1000066013,
    R3_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
    R3_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
    R3_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
    R3_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
    R3_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG = 1000054004,
    R3_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG = 1000054005,
    R3_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG = 1000054006,
    R3_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG = 1000054007,
};

enum PresentMode {
    R3_PRESENT_MODE_IMMEDIATE = 0,
    R3_PRESENT_MODE_MAILBOX = 1,
    R3_PRESENT_MODE_FIFO = 2,
    R3_PRESENT_MODE_FIFO_RELAXED = 3,
    R3_PRESENT_MODE_SHARED_DEMAND_REFRESH = 1000111000,
    R3_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH = 1000111001,
};

enum ColorSpace {
    R3_COLOR_SPACE_SRGB_NONLINEAR = 0,
    R3_COLOR_SPACE_DISPLAY_P3_NONLINEAR = 1000104001,
    R3_COLOR_SPACE_ENDED_SRGB_LINEAR = 1000104002,
    R3_COLOR_SPACE_DISPLAY_P3_LINEAR = 1000104003,
    R3_COLOR_SPACE_DCI_P3_NONLINEAR = 1000104004,
    R3_COLOR_SPACE_BT709_LINEAR = 1000104005,
    R3_COLOR_SPACE_BT709_NONLINEAR = 1000104006,
    R3_COLOR_SPACE_BT2020_LINEAR = 1000104007,
    R3_COLOR_SPACE_HDR10_ST2084 = 1000104008,
    R3_COLOR_SPACE_DOLBYVISION = 1000104009,
    R3_COLOR_SPACE_HDR10_HLG = 1000104010,
    R3_COLOR_SPACE_ADOBERGB_LINEAR = 1000104011,
    R3_COLOR_SPACE_ADOBERGB_NONLINEAR = 1000104012,
    R3_COLOR_SPACE_PASS_THROUGH = 1000104013,
    R3_COLOR_SPACE_ENDED_SRGB_NONLINEAR = 1000104014,
    R3_COLOR_SPACE_DISPLAY_NATIVE_AMD = 1000213000,
};

} // namespace R3